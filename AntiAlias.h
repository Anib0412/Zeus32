/////////////////////////////////////////////////////////////////////////////
// AntiAlias.h
//                                                                        
// anti-aliasing for Rip Process for RasterPlus95 project
// Copyright (C) 1996 Graphx, Inc.
//

#ifndef __ANTIALIAS_H__
#define __ANTIALIAS_H__

class CAntiAlias
{
public:
    CAntiAlias();
    ~CAntiAlias();
	void SetOverSample(int os);
    void boxfilterraster8(BYTE* pInputBits, BYTE* pOutputBits, 
       int InputWidth, int InputHeight);
    void boxfilterraster24(BYTE* pInputBits, BYTE* pOutputBits, 
       int InputWidth, int InputHeight);
    void boxfilterraster32(BYTE* pInputBits, BYTE* pOutputBits, 
       int InputWidth, int InputHeight);
	void InitTriangleFilter();
	void trifilterraster(BYTE* pInputBits, BYTE* pOutputBits, 
       int InputWidth, int InputHeight);
       
protected:
    float* Triangle_Coeffs;
	int OverSample;
};				   

#endif
