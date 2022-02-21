#define WIN32_LEAN_AND_MEAN 
#include <stbhandler.h>

#define STBI_ONLY_PSD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// very simple plugin that loads PSD files using stb_image header only library

static BOOL AcceptSTBImage(const unsigned char* buffer, unsigned int size) { 
    return TRUE; 
}

const char* GetSTBDescription() { return "PSD Images"; }
const char* GetSTBExtension() { return "*.PSD"; }

struct stb_internal {
    unsigned char* data;
};

static BOOL OpenSTBImage(ImagePtr img, const TCHAR* name) {
    struct stb_internal* new_internal = (struct stb_internal*)MYALLOC(sizeof(struct stb_internal));
	new_internal->data = 0;
    img->handler->internal = (void*)new_internal;
    img->numdirs = 1;
    img->supportmt = 0;
    img->currentdir = 0;
    return TRUE;
}

static void SetSTBDirectory(ImagePtr img, unsigned int which) {
    struct stb_internal* stb_internal = (struct stb_internal*)img->handler->internal;
	
	int w, h, c;
	unsigned char* data = stbi_load(img->name, &w, &h, &c, 4);
	if (data) {
        img->width = w;
        img->height  = h;
		stb_internal->data = data;
        img->numtilesx = 1;
		img->numtilesy = 1;
		img->twidth = img->width;
		img->theight = img->height;
		img->subfiletype = Normal;
        img->istiled = FALSE;
    }
 }

static HBITMAP
LoadSTBTile(ImagePtr img, HDC hdc, unsigned int x, unsigned int y) {
    struct stb_internal* stb_internal = (struct stb_internal*)img->handler->internal;
    HBITMAP hbitmap = 0;    
    unsigned int* bits = 0;
    hbitmap = img->helper.CreateTrueColorDIBSection(hdc, img->twidth, -(int)img->theight, &bits, 32);
    if (bits) {
		memcpy(bits, stb_internal->data, img->twidth * img->theight * 4);
    }
    return hbitmap;
}

static void CloseSTBImage(ImagePtr img) {
    if (img->handler && img->handler->internal) {
        struct stb_internal* stb_internal = (struct stb_internal*)img->handler->internal;
		free(stb_internal->data);
        MYFREE(stb_internal);
    }
}

void RegisterVlivPlugin(ImagePtr img) {
    img->helper.Register(AcceptSTBImage,
			 GetSTBDescription,
			 GetSTBExtension,
			 OpenSTBImage,
			 SetSTBDirectory,
			 LoadSTBTile,
			 CloseSTBImage);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                   DWORD  ul_reason_for_call, 
                   LPVOID lpReserved
                 ) {
    return TRUE;
}
