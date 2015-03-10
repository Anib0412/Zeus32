/////////////////////////////////////////////////////////////////////////////
// CTM2RECT.CPP
//                                                                        
// convert CTM to a Rect for sizing for RasterPlus95
// Copyright (C) 1996-97 Graphx, Inc.
//
// mods:
// 6-09-97  Build 50 - added stuff for non-square resolutions
// 1-06-98  Version 2.0.1 - changed ApplySizingValsToCRect so that returned rect
//                          is passed on stack as a reference
// 1-06-98  Version 2.0.1 - took off optimization because of xform problems
//

#include"ctm2rect.h"
#include"math.h"

// in order to make the visual sizing fly, we need to
// convert the sizing vals to a transformation matrix.  
// We'll use this matrix to create a set of linear equations 
// which will let us transform an input bounding rect into a
// sized rect.
//
// the vals are: xzoom, yzoom, xoff, yoff, and rotation
//
// the matrix we'll create is a standard linear transformation
// matrix, many examples of which are in the PostScript Red
// book.  To whit:
//
//    | a  b  0 |
//    | c  d  0 |
//    | tx ty 1 |
//
// and the matching linear equations are:
//
// x2 = a * x + c * y + tx
// y2 = b * x + d * y + ty
//
// note that the first cut will only support rotation increments 
// of 90 degrees which greatly simplifies the linear equations
// because val[1] and val[2] drop out

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTM2Rect::CTM2Rect()
   {
   }
   
CTM2Rect::~CTM2Rect()
   {
   }

void CTM2Rect::ApplySizingValsToCRect(CRect& SizedRect, PSIZING_VALS pSV, const CRect& BoundingRect, 
									   POINT& ptRotateCorrect, float& PicaWidth,
									   float& PicaHeight)
   {   
   CRect NewRect, OldRect;   
   CTM_ARRAY first, second, Product;
   double angle;     
   double iHeight, iWidth;
   double xorg, yorg, xnew, ynew, radius;
   int pass = 1;

   // convert to radians
   //angle = pSV->rotation * PIVALUE / 180.0;
   angle = 0;

doagain:

   // height and width of page
   iWidth = (double)BoundingRect.Width();
   iHeight = (double)BoundingRect.Height();
   // center point of page
   xorg = iWidth / 2;
   yorg = iHeight / 2;
   // distance from center point to origin
   radius = sqrt(yorg * yorg + xorg * xorg);

   // compute where the center will be after we rotate it
   xnew = cos(angle + asin(yorg / radius)) * radius;
   ynew = sin(angle + asin(yorg / radius)) * radius;
   
   // array #1 is the rotation
   first.val[0] = (float)cos(angle);
   first.val[1] = (float)sin(angle);
   first.val[2] = (float)(-sin(angle));
   first.val[3] = (float)cos(angle);
   first.val[4] = 0.0F;
   first.val[5] = 0.0F;

   // array #2 moves the center of the image back to the 
   // original point
   second.val[0] = 1.0F;
   second.val[1] = 0.0F;
   second.val[2] = 0.0F;
   second.val[3] = 1.0F;
   second.val[4] = (float)(xorg - xnew);
   second.val[5] = (float)(yorg - ynew);
   
   GetMatrixProduct(&first, &second, &Product);   
   first = Product;				   

   // load up second matrix
   second.val[0] = pSV->xzoom;
   second.val[1] = 0.0F;
   second.val[2] = 0.0F;
   second.val[3] = pSV->yzoom;
   // convert these from pica to pixels
   second.val[4] = pSV->xoff * (float)(BoundingRect.Width()) / PicaWidth;
   second.val[5] = pSV->yoff * (float)(BoundingRect.Height()) / PicaHeight;

   // get the CTM array
   GetMatrixProduct(&first, &second, &Product);
   
   // now convert to Windows coordinate system...    
   // rotate 180 degrees   
   first = Product;    
   second.val[0] = (float)cos(PIVALUE);
   second.val[1] = (float)sin(PIVALUE);
   second.val[2] = (float)(-sin(PIVALUE));
   second.val[3] = (float)cos(PIVALUE);
   second.val[4] = 0.0F;
   second.val[5] = 0.0F;        
   // get the CTM array
   GetMatrixProduct(&first, &second, &Product);      
      
   // flip along the x axis
   // and move up a distance equal to height
   first = Product; 
   second.val[0] = -1.0F;
   second.val[1] = 0.0F;
   second.val[2] = 0.0F;
   second.val[3] = 1.0F;
   second.val[4] = 0.0F;
   second.val[5] = (float)BoundingRect.Height();      
   // get the CTM array
   GetMatrixProduct(&first, &second, &Product);

   // compute SizedRect                                         
   POINT ll, ul, lr, ur;
   POINT ll_new, ul_new, lr_new, ur_new;
   ll.x = BoundingRect.left;
   ul.x = BoundingRect.left;
   lr.x = BoundingRect.right;
   ur.x = BoundingRect.right;
   ll.y = BoundingRect.bottom;
   ul.y = BoundingRect.top;
   lr.y = BoundingRect.bottom;
   ur.y = BoundingRect.top;

   // convert to new coords
   ll_new.x = (int)((float)Product.val[0] * (float)ll.x + 
      (float)Product.val[2] * (float)ll.y + (float)Product.val[4] + .5F);
   ll_new.y = (int)((float)Product.val[1] * (float)ll.x + 
      (float)Product.val[3] * (float)ll.y + (float)Product.val[5] + .5F);
   ul_new.x = (int)((float)Product.val[0] * (float)ul.x + 
      (float)Product.val[2] * (float)ul.y + (float)Product.val[4] + .5F);
   ul_new.y = (int)((float)Product.val[1] * (float)ul.x + 
      (float)Product.val[3] * (float)ul.y + (float)Product.val[5] + .5F);
   lr_new.x = (int)((float)Product.val[0] * (float)lr.x + 
      (float)Product.val[2] * (float)lr.y + (float)Product.val[4] + .5F);
   lr_new.y = (int)((float)Product.val[1] * (float)lr.x + 
      (float)Product.val[3] * (float)lr.y + (float)Product.val[5] + .5F);
   ur_new.x = (int)((float)Product.val[0] * (float)ur.x + 
      (float)Product.val[2] * (float)ur.y + (float)Product.val[4] + .5F);
   ur_new.y = (int)((float)Product.val[1] * (float)ur.x + 
      (float)Product.val[3] * (float)ur.y + (float)Product.val[5] + .5F);

   if (pass == 1)
      {
      OldRect.left = min(min(min(ll_new.x, ul_new.x), ur_new.x), lr_new.x);
      OldRect.right = max(max(max(ll_new.x, ul_new.x), ur_new.x), lr_new.x);
      OldRect.bottom = min(min(min(ll_new.y, ul_new.y), ur_new.y), lr_new.y);
      OldRect.top = max(max(max(ll_new.y, ul_new.y), ur_new.y), lr_new.y);   
	  pass = 2;
	  angle = pSV->rotation * PIVALUE / 180.0;
	  goto doagain;
	  }

   SizedRect.left = min(min(min(ll_new.x, ul_new.x), ur_new.x), lr_new.x);
   SizedRect.right = max(max(max(ll_new.x, ul_new.x), ur_new.x), lr_new.x);
   SizedRect.bottom = min(min(min(ll_new.y, ul_new.y), ur_new.y), lr_new.y);
   SizedRect.top = max(max(max(ll_new.y, ul_new.y), ur_new.y), lr_new.y);
   
   // we need this to correct canvas offsets   
   OldRect.NormalizeRect();
   NewRect = SizedRect;
   NewRect.NormalizeRect();
   ptRotateCorrect.x = NewRect.left - OldRect.left;
   ptRotateCorrect.y = NewRect.bottom - OldRect.bottom;   
   }

   
/***************************************************************************/
/*                           GetMatrixProduct                              */
/*                                                                         */
/* multiply two matricies and store product in third matrix                */
/*                                                                         */
/* | a    b    0 |        | a1   b1   0 |                                  */
/* | c    d    0 |    X   | c1   d1   0 |                                  */
/* | tx   ty   1 |        | tx1  ty1  1 |     results in:                  */
/*                                                                         */
/* | a * a1 + b * c1          a * b1 + b * d1          0 |                 */
/* | c * a1 + d * c1          c * b1 + d * d1          0 |                 */
/* | tx * a1 + ty * c1 + tx1  tx * b1 + ty * d1 + ty1  1 |                 */
/*                                                                         */
/***************************************************************************/

void CTM2Rect::GetMatrixProduct(CTM_ARRAY *first, CTM_ARRAY *second, CTM_ARRAY *product)
   {
   product->val[0] =  first->val[0] * second->val[0] +
		      first->val[1] * second->val[2];

   product->val[1] =  first->val[0] * second->val[1] +
		      first->val[1] * second->val[3];

   product->val[2] =  first->val[2] * second->val[0] +
		      first->val[3] * second->val[2];

   product->val[3] =  first->val[2] * second->val[1] +
		      first->val[3] * second->val[3];

   product->val[4] =  first->val[4] * second->val[0] +
		      first->val[5] * second->val[2] +
		      second->val[4];

   product->val[5] =  first->val[4] * second->val[1] +
		      first->val[5] * second->val[3] +
		      second->val[5];
   }
