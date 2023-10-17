# Fonts management

## Fonts format
MuditaOS uses its own fonts binary format with extension .mpf.
Fonts can be converted from popular formats (ie otf, ttf) to this format via converter tool: https://github.com/mudita/fontbuilder

Reasons to use specific binary format:
- reduce workload on CPU since it has no GPU unit
- reduce size of fonts both on disk and in RAM
## Fonts source
Fonts are assets which are downloaded from separate repositories using `download_assets.py` script (see more: [download assets documentation](download_assets.md)). Basing on configuration (Community/Proprietary) fonts will be downloaded from two different repositories.

## Fonts mapping
Regardless of the source, both assets packages contain file fontmap.json, mapping font files to font types used in MuditaOS code.

Full example can be found in public assets repo: https://github.com/mudita/MuditaOSPublicAssets/blob/master/image/assets/fonts/fontmap.json

Short example:
```
{
    "info" : {
        "fallback_font" : "dejavu_sans_bold_27",
        "default_font": "gt_pressura_bold_27",
        "default_font_family" : "gt_pressura"
    },
    "style": {
        "small":      "gt_pressura/gt_pressura_regular_24.mpf",
        ...
    }
}
```
There are three obligatory values in `info` part:
- `fallback_font` - fallback font used when specified font is unable to display some glyphs
- `default_font` - default font used in case of inability to load specified font
- `default_font_family` - default font family name used in methods which specify only size and weight of a font


Every key in `style` part defines font type to be used in fonts list in MuditaOS, which later can be used in `FontManager::getFont` method (ie `getFont("supersizemelight")`)

## Fonts usage in MuditaOS

FontManager loads all fonts based on `fontmap.json`. These can be later accessed in GUI methods, specifically via: 
- `getFont(std::string fontType)` -  returns font as `RawFont*` basing on font type (ie `verysmall`, `supersizemelight`)
- `getFontByName(std::string fontName)` -  returns font as `RawFont*` basing on font name from font descriptor (ie. `dejavu_sans_regular_20`, `dejavu_sans_light_90`)

