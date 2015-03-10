#ifndef __USERFILE_ 
#define __USERFILE_ 

#include "..\CmnLibs\Current LeadTools\include\Classlib\LtWrappr.h"
#include "..\CmnLibs\Current LeadTools\include\Classlib\Ltcfile.h"

class LUserFile : public LFile
{
 LEAD_DECLAREOBJECT(LUserFile);

public:
	L_INT       m_nDispRow ;
	FILEINFO    m_FileInfo;
	LBuffer*    m_pLeadBuffer;
	LUserFile();
   L_INT  LoadFileCallBack(pFILEINFO pFileInfo, LBitmapBase * pLBitmap, 
									 LBuffer * pLBuffer, L_UINT uFlags,
									 L_INT nRow, L_INT nLines);

	virtual ~LUserFile();
};

#endif //__USERFILE_ 