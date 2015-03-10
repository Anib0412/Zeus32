#ifndef __RIPPANE_H__
#define __RIPPANE_H__

class CPaneElement : public CObject
   {
DECLARE_DYNAMIC(CPaneElement)
public:
   CPaneElement() {}
   ~CPaneElement() {}
   void* pDocPtr;
   int iViewStyle;
   void* pcstPtr;
   BOOL InRip;
   };

class CPaneArray : public CObArray
   {
public:      
   BOOL AnyRipActive();
   void Destroy();   
   void DeleteElement(CPaneElement* pPassedElement);
   };

#endif