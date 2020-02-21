#include "Image.h"

namespace mob
{
    using std::wstring;

    Image::Image(wstring name) :
        handle_(0) {
        handle_ = LoadImageW(
            nullptr,
            name.c_str(),
            IMAGE_ICON,
            0,
            0,
            LR_LOADFROMFILE
        );

    }

    Image::~Image() {
        DestroyIcon( (HICON)handle_ );
    }

} // micbinz
