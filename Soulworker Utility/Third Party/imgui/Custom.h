#pragma once

using namespace std;
#define OUTLINE_TEXT_USE_FUNCTION_SIZE_UP 0

namespace ImGui {

    typedef struct _IMGUIOUTLINETEXT {
        ImU32 _color;
        FLOAT _size;
        _IMGUIOUTLINETEXT(ImU32 color, FLOAT size) : _color(color), _size(size) {}
    }IMGUIOUTLINETEXT;

    class OutlineText {
    private:
        static vector<IMGUIOUTLINETEXT> _outlineColor;
        
    public:
        OutlineText() {}
        ~OutlineText() { _outlineColor.clear(); }

        static VOID PushOutlineText(IMGUIOUTLINETEXT info) {_outlineColor.push_back(info);}
        static VOID PopOutlineText() { _outlineColor.pop_back(); }
        static BOOL CheckOutlineTextFlag(){ return !_outlineColor.empty(); }
        static IMGUIOUTLINETEXT* GetOutlineColor() {
            if (_outlineColor.empty())
                return nullptr;
            else
                return &*(_outlineColor.end() - 1);
        }
    };

    class TextAlignCenter {
    private:
        static BOOL _isSetAlignCenter;

    public:
        TextAlignCenter(){}
        ~TextAlignCenter(){}

        static VOID SetTextAlignCenter() { _isSetAlignCenter = TRUE; }
        static VOID UnSetTextAlignCenter() { _isSetAlignCenter = FALSE; }
        static BOOL CheckAlignCenter() { return _isSetAlignCenter; }
    };
}
