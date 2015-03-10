// color stuff
#ifdef __cplusplus
extern "C"
{
#endif
int WINAPIV colorcorrect_rgb2cmyk(DWORD pCWnd, unsigned char*, unsigned char*, int);
int WINAPIV colorcorrect_cmyk2cmyk(DWORD pCWnd, unsigned char*, unsigned char*, int);
int WINAPIV colorcorrect_rgb2rgb(DWORD pCWnd, unsigned char*, unsigned char*, int);
int WINAPIV colorcorrect_cmyk2rgb(DWORD pCWnd, unsigned char*, unsigned char*, int);
#ifdef __cplusplus
   }
#endif
