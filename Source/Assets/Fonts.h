#pragma once

namespace Silent::Utils{ struct FontMetadata; }

using namespace Silent::Utils;

namespace Silent::Assets
{
    constexpr char GLYPH_PRECACHE[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
                                      "ÁÀÂĄÄÆĆÇÉÈÊËĘĞİÍÌÎÏŁŃÑÓÔÖØŒŚŞÚÙÛÜŸŹŻáàâąäæćçéèêëęğıíìîïłńñóôöøœßśşúùûüÿźż"
                                      "“”«»¿¡★"
                                      "あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゆよらりるれろわをん"
                                      "アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤユヨラリルレロワヲン"
                                      "０１２３４５６７８９ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ"
                                      "「」。！？、";

    extern const std::vector<FontMetadata> FONTS_METADATA;
}
