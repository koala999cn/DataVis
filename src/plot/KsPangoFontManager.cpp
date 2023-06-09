#include "KsPangoFontManager.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"


namespace kPrivate
{
    struct KpFont_
    {
        PangoFontFamily* family;
        bool hasBoldFace;
        bool hasObliqueFace;
    };
}


KsPangoFontManager::KsPangoFontManager()
{
    auto fontmap = pango_cairo_font_map_new();
    PangoFontFamily** families; int nfamilies;
    pango_font_map_list_families(fontmap, &families, &nfamilies);

    families_.reserve(nfamilies);
    for (int i = 0; i < nfamilies; i++) {
        auto name = pango_font_family_get_name(families[i]);
        families_.push_back(name);
        auto font = new kPrivate::KpFont_;
        font->family = families[i];
        font->hasBoldFace = true;
        font->hasObliqueFace = true;
        familiesMap_.insert({ name, font });
    }

    g_free(families);
}


KsPangoFontManager::~KsPangoFontManager()
{
    for each (auto& var in familiesMap_) 
        delete var.second;
}
