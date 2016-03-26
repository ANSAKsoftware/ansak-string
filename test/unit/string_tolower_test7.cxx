///////////////////////////////////////////////////////////////////////////
//
// 2015.10.16
//
// The author (Arthur N. Klassen) disclaims all copyright to this source code.
// In place of a legal notice, here is a blessing:
//
//    May you do good and not evil.
//    May you find forgiveness for yourself and forgive others.
//    May you share freely, never taking more than you give.
//
// (Is it too much to suggest retaining this header on this file?)
///////////////////////////////////////////////////////////////////////////
//
// string_tolower_test.cxx -- CPPUNIT-compatible tests to exercise string.hxx/.cxx
//                   If you find new conditions that I missed, please let
//                   me make my version of this better, too.
//
///////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "string.hxx"
#include "string_internal.hxx"
#include <string.h>

using namespace ansak;
using namespace ansak::internal;
using namespace std;
using namespace testing;

TEST(StringToLowerTest, testEightBitToLowers)
{
    string upperCase("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG");
    string lowerCase("the quick brown fox jumps over the lazy dog");
    string empty(toLower(""));

    EXPECT_EQ(lowerCase, toLower(upperCase));
    EXPECT_NE(lowerCase, toLower(upperCase, "tur"));
    EXPECT_TRUE(empty.empty());
}

TEST(StringToLowerTest, testTurkishToLowers)
{
    string upperCaseT("THE QUICK BROWN F\xc4\xb0X JUMPS OVER THE LAZY DOG");
    string lowerCaseT("the qu\xc4\xb1" "ck brown fix jumps over the lazy dog");
    string lowerCaseNonT("the quick brown fix jumps over the lazy dog");

    EXPECT_NE(lowerCaseT, toLower(upperCaseT));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "tr"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "tur"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "az"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "aze"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "azb"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "azj"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "tt"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "tat"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "kk"));
    EXPECT_EQ(lowerCaseT, toLower(upperCaseT, "kaz"));
    EXPECT_EQ(lowerCaseNonT, toLower(upperCaseT, "de"));
    EXPECT_EQ(lowerCaseNonT, toLower(upperCaseT, "engl"));

    char16_t upperCaseT16Src[] = { 'T', 'H', 'E', ' ', 'Q', 'U', 'I', 'C', 'K', ' ',
                                   'B', 'R', 'O', 'W', 'N', ' ', 'F', 0x130, 'X', ' ',
                                   'J', 'U', 'M', 'P', 'S', ' ', 'O', 'V', 'E', 'R',
                                   ' ', 'T', 'H', 'E', ' ', 'L', 'A', 'Z', 'Y', ' ',
                                   'D', 'O', 'G', 0 };
    char16_t lowerCaseT16Src[] = { 't', 'h', 'e', ' ', 'q', 'u', 0x131, 'c', 'k', ' ',
                                   'b', 'r', 'o', 'w', 'n', ' ', 'f', 'i', 'x', ' ',
                                   'j', 'u', 'm', 'p', 's', ' ', 'o', 'v', 'e', 'r', ' ',
                                   't', 'h', 'e', ' ', 'l', 'a', 'z', 'y', ' ',
                                   'd', 'o', 'g', 0 };
    ucs2String upperCaseT16(upperCaseT16Src);
    ucs2String lowerCaseT16(lowerCaseT16Src);

    EXPECT_NE(lowerCaseT16, toLower(upperCaseT16));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "tr"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "tur"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "az"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "aze"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "azb"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "azj"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "tt"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "tat"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "kk"));
    EXPECT_EQ(lowerCaseT16, toLower(upperCaseT16, "kaz"));

    char32_t upperCaseT32Src[] = { 'T', 'H', 'E', ' ', 'Q', 'U', 'I', 'C', 'K', ' ',
                                   'B', 'R', 'O', 'W', 'N', ' ', 'F', 0x130, 'X', ' ',
                                   'J', 'U', 'M', 'P', 'S', ' ', 'O', 'V', 'E', 'R',
                                   ' ', 'T', 'H', 'E', ' ', 'L', 'A', 'Z', 'Y', ' ',
                                   'D', 'O', 'G', 0 };
    char32_t lowerCaseT32Src[] = { 't', 'h', 'e', ' ', 'q', 'u', 0x131, 'c', 'k', ' ',
                                   'b', 'r', 'o', 'w', 'n', ' ', 'f', 'i', 'x', ' ',
                                   'j', 'u', 'm', 'p', 's', ' ', 'o', 'v', 'e', 'r', ' ',
                                   't', 'h', 'e', ' ', 'l', 'a', 'z', 'y', ' ',
                                   'd', 'o', 'g', 0 };
    ucs4String upperCaseT32(upperCaseT32Src);
    ucs4String lowerCaseT32(lowerCaseT32Src);

    EXPECT_NE(lowerCaseT32, toLower(upperCaseT32));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "tr"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "tur"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "az"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "aze"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "azb"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "azj"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "tt"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "tat"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "kk"));
    EXPECT_EQ(lowerCaseT32, toLower(upperCaseT32, "kaz"));
}

TEST(StringToLowerTest, testSixteenBitToLowers)
{
    ucs2String empty;
    EXPECT_TRUE(toLower(empty).empty());
    ucs2String
      u00(u"\u0041\u0042\u0043\u0044\u0045\u0046\u0047\u0048\u0049\u004A\u004B\u004C\u004D\u004E\u004F\u0050\u0051\u0052\u0053\u0054\u0055"),
      u01(u"\u0056\u0057\u0058\u0059\u005A\u00C0\u00C1\u00C2\u00C3\u00C4\u00C5\u00C6\u00C7\u00C8\u00C9\u00CA\u00CB\u00CC\u00CD\u00CE\u00CF"),
      u02(u"\u00D0\u00D1\u00D2\u00D3\u00D4\u00D5\u00D6\u00D8\u00D9\u00DA\u00DB\u00DC\u00DD\u00DE\u0100\u0104\u0106\u0108\u010A\u010C\u010E"),
      u03(u"\u0110\u0112\u0114\u0116\u0118\u011A\u011C\u011E\u0120\u0122\u0124\u0126\u0128\u012A\u012C\u012E\u0130\u0132\u0134\u0136\u0139"),
      u04(u"\u013B\u013D\u013F\u0141\u0143\u0145\u0147\u014A\u014C\u014E\u0150\u0152\u0154\u0156\u0158\u015A\u015C\u015E\u0160\u0162\u0164"),
      u05(u"\u0166\u0168\u016A\u016C\u016E\u0170\u0172\u0174\u0176\u0178\u0179\u017B\u017D\u0181\u0182\u0184\u0186\u0187\u0189\u018A\u018B"),
      u06(u"\u018E\u018F\u0190\u0191\u0193\u0194\u0196\u0197\u0198\u019C\u019D\u019F\u01A0\u01A2\u01A4\u01A6\u01A7\u01A9\u01AC\u01AE\u01AF"),
      u07(u"\u01B1\u01B2\u01B3\u01B5\u01B7\u01B8\u01BC\u01C4\u01C5\u01C7\u01C8\u01CA\u01CB\u01CD\u01CF\u01D1\u01D3\u01D5\u01D7\u01D9\u01DB"),
      u08(u"\u01DE\u01E0\u01E2\u01E4\u01E6\u01E8\u01EA\u01EC\u01EE\u01F1\u01F2\u01F4\u01F6\u01F7\u01F8\u01FA\u01FC\u01FE\u0200\u0202\u0204"),
      u09(u"\u0206\u0208\u020A\u020C\u020E\u0210\u0212\u0214\u0216\u0218\u021A\u021C\u021E\u0220\u0222\u0224\u0226\u0228\u022A\u022C\u022E"),
      u10(u"\u0230\u0232\u023A\u023B\u023D\u023E\u0241\u0243\u0244\u0245\u0246\u0248\u024A\u024C\u024E\u0370\u0372\u0376\u037F\u0386\u0388"),
      u11(u"\u0389\u038A\u038C\u038E\u038F\u0391\u0392\u0393\u0394\u0395\u0396\u0397\u0398\u0399\u039A\u039B\u039C\u039D\u039E\u039F\u03A0"),
      u12(u"\u03A1\u03A3\u03A4\u03A5\u03A6\u03A7\u03A8\u03A9\u03AA\u03AB\u03CF\u03D8\u03DA\u03DC\u03DE\u03E0\u03E2\u03E4\u03E6\u03E8\u03EA"),
      u13(u"\u03EC\u03EE\u03F4\u03F7\u03F9\u03FA\u03FD\u03FE\u03FF\u0400\u0401\u0402\u0403\u0404\u0405\u0406\u0407\u0408\u0409\u040A\u040B"),
      u14(u"\u040C\u040D\u040E\u040F\u0410\u0411\u0412\u0413\u0414\u0415\u0416\u0417\u0418\u0419\u041A\u041B\u041C\u041D\u041E\u041F\u0420"),
      u15(u"\u0421\u0422\u0423\u0424\u0425\u0426\u0427\u0428\u0429\u042A\u042B\u042C\u042D\u042E\u042F\u0460\u0462\u0464\u0466\u0468\u046A"),
      u16(u"\u046C\u046E\u0470\u0472\u0474\u0476\u0478\u047A\u047C\u047E\u0480\u048A\u048C\u048E\u0490\u0492\u0494\u0496\u0498\u049A\u049C"),
      u17(u"\u049E\u04A0\u04A2\u04A4\u04A6\u04A8\u04AA\u04AC\u04AE\u04B0\u04B2\u04B4\u04B6\u04B8\u04BA\u04BC\u04BE\u04C0\u04C1\u04C3\u04C5"),
      u18(u"\u04C7\u04C9\u04CB\u04CD\u04D0\u04D2\u04D4\u04D6\u04D8\u04DA\u04DC\u04DE\u04E0\u04E2\u04E4\u04E6\u04E8\u04EA\u04EC\u04EE\u04F0"),
      u19(u"\u04F2\u04F4\u04F6\u04F8\u04FA\u04FC\u04FE\u0500\u0502\u0504\u0506\u0508\u050A\u050C\u050E\u0510\u0512\u0514\u0516\u0518\u051A"),
      u20(u"\u051C\u051E\u0520\u0522\u0524\u0526\u0528\u052A\u052C\u052E\u0531\u0532\u0533\u0534\u0535\u0536\u0537\u0538\u0539\u053A\u053B"),
      u21(u"\u053C\u053D\u053E\u053F\u0540\u0541\u0542\u0543\u0544\u0545\u0546\u0547\u0548\u0549\u054A\u054B\u054C\u054D\u054E\u054F\u0550"),
      u22(u"\u0551\u0552\u0553\u0554\u0555\u0556\u10A0\u10A1\u10A2\u10A3\u10A4\u10A5\u10A6\u10A7\u10A8\u10A9\u10AA\u10AB\u10AC\u10AD\u10AE"),
      u23(u"\u10AF\u10B0\u10B1\u10B2\u10B3\u10B4\u10B5\u10B6\u10B7\u10B8\u10B9\u10BA\u10BB\u10BC\u10BD\u10BE\u10BF\u10C0\u10C1\u10C2\u10C3"),
      u24(u"\u10C4\u10C5\u10C7\u10CD\u1E00\u1E02\u1E04\u1E06\u1E08\u1E0A\u1E0C\u1E0E\u1E10\u1E12\u1E14\u1E16\u1E18\u1E1A\u1E1C"),
      u29(u"\u1E1E\u1E20\u1E22\u1E24\u1E26\u1E28\u1E2A\u1E2C\u1E2E\u1E30\u1E32\u1E34\u1E36\u1E38\u1E3A\u1E3C\u1E3E\u1E40\u1E42\u1E44\u1E46"),
      u30(u"\u1E48\u1E4A\u1E4C\u1E4E\u1E50\u1E52\u1E54\u1E56\u1E58\u1E5A\u1E5C\u1E5E\u1E60\u1E62\u1E64\u1E66\u1E68\u1E6A\u1E6C\u1E6E\u1E70"),
      u31(u"\u1E72\u1E74\u1E76\u1E78\u1E7A\u1E7C\u1E7E\u1E80\u1E82\u1E84\u1E86\u1E88\u1E8A\u1E8C\u1E8E\u1E90\u1E92\u1E94\u1E9E\u1EA0\u1EA2"),
      u32(u"\u1EA4\u1EA6\u1EA8\u1EAA\u1EAC\u1EAE\u1EB0\u1EB2\u1EB4\u1EB6\u1EB8\u1EBA\u1EBC\u1EBE\u1EC0\u1EC2\u1EC4\u1EC6\u1EC8\u1ECA\u1ECC"),
      u33(u"\u1ECE\u1ED0\u1ED2\u1ED4\u1ED6\u1ED8\u1EDA\u1EDC\u1EDE\u1EE0\u1EE2\u1EE4\u1EE6\u1EE8\u1EEA\u1EEC\u1EEE\u1EF0\u1EF2\u1EF4\u1EF6"),
      u34(u"\u1EF8\u1EFA\u1EFC\u1EFE\u1F08\u1F09\u1F0A\u1F0B\u1F0C\u1F0D\u1F0E\u1F0F\u1F18\u1F19\u1F1A\u1F1B\u1F1C\u1F1D\u1F28\u1F29\u1F2A"),
      u35(u"\u1F2B\u1F2C\u1F2D\u1F2E\u1F2F\u1F38\u1F39\u1F3A\u1F3B\u1F3C\u1F3D\u1F3E\u1F3F\u1F48\u1F49\u1F4A\u1F4B\u1F4C\u1F4D\u1F59\u1F5B"),
      u36(u"\u1F5D\u1F5F\u1F68\u1F69\u1F6A\u1F6B\u1F6C\u1F6D\u1F6E\u1F6F\u1F88\u1F89\u1F8A\u1F8B\u1F8C\u1F8D\u1F8E\u1F8F\u1F98\u1F99\u1F9A"),
      u37(u"\u1F9B\u1F9C\u1F9D\u1F9E\u1F9F\u1FA8\u1FA9\u1FAA\u1FAB\u1FAC\u1FAD\u1FAE\u1FAF\u1FB8\u1FB9\u1FBA\u1FBB\u1FBC\u1FC8\u1FC9\u1FCA"),
      u38(u"\u1FCB\u1FCC\u1FD8\u1FD9\u1FDA\u1FDB\u1FE8\u1FE9\u1FEA\u1FEB\u1FEC\u1FF8\u1FF9\u1FFA\u1FFB\u1FFC\u2126\u212A\u212B\u2132\u2160"),
      u39(u"\u2161\u2162\u2163\u2164\u2165\u2166\u2167\u2168\u2169\u216A\u216B\u216C\u216D\u216E\u216F\u2183\u24B6\u24B7\u24B8\u24B9\u24BA"),
      u40(u"\u24BB\u24BC\u24BD\u24BE\u24BF\u24C0\u24C1\u24C2\u24C3\u24C4\u24C5\u24C6\u24C7\u24C8\u24C9\u24CA\u24CB\u24CC\u24CD\u24CE\u24CF"),
      u41(u"\u2C00\u2C01\u2C02\u2C03\u2C04\u2C05\u2C06\u2C07\u2C08\u2C09\u2C0A\u2C0B\u2C0C\u2C0D\u2C0E\u2C0F\u2C10\u2C11\u2C12\u2C13\u2C14"),
      u42(u"\u2C15\u2C16\u2C17\u2C18\u2C19\u2C1A\u2C1B\u2C1C\u2C1D\u2C1E\u2C1F\u2C20\u2C21\u2C22\u2C23\u2C24\u2C25\u2C26\u2C27\u2C28\u2C29"),
      u43(u"\u2C2A\u2C2B\u2C2C\u2C2D\u2C2E\u2C60\u2C62\u2C63\u2C64\u2C67\u2C69\u2C6B\u2C6D\u2C6E\u2C6F\u2C70\u2C72\u2C75\u2C7E\u2C7F\u2C80"),
      u44(u"\u2C82\u2C84\u2C86\u2C88\u2C8A\u2C8C\u2C8E\u2C90\u2C92\u2C94\u2C96\u2C98\u2C9A\u2C9C\u2C9E\u2CA0\u2CA2\u2CA4\u2CA6\u2CA8\u2CAA"),
      u45(u"\u2CAC\u2CAE\u2CB0\u2CB2\u2CB4\u2CB6\u2CB8\u2CBA\u2CBC\u2CBE\u2CC0\u2CC2\u2CC4\u2CC6\u2CC8\u2CCA\u2CCC\u2CCE\u2CD0\u2CD2\u2CD4"),
      u46(u"\u2CD6\u2CD8\u2CDA\u2CDC\u2CDE\u2CE0\u2CE2\u2CEB\u2CED\u2CF2\uA640\uA642\uA644\uA646\uA648\uA64A\uA64C\uA64E\uA650\uA652\uA654"),
      u47(u"\uA656\uA658\uA65A\uA65C\uA65E\uA660\uA662\uA664\uA666\uA668\uA66A\uA66C\uA680\uA682\uA684\uA686\uA688\uA68A\uA68C\uA68E\uA690"),
      u48(u"\uA692\uA694\uA696\uA698\uA69A\uA722\uA724\uA726\uA728\uA72A\uA72C\uA72E\uA732\uA734\uA736\uA738\uA73A\uA73C\uA73E\uA740\uA742"),
      u49(u"\uA744\uA746\uA748\uA74A\uA74C\uA74E\uA750\uA752\uA754\uA756\uA758\uA75A\uA75C\uA75E\uA760\uA762\uA764\uA766\uA768\uA76A\uA76C"),
      u50(u"\uA76E\uA779\uA77B\uA77D\uA77E\uA780\uA782\uA784\uA786\uA78B\uA78D\uA790\uA792\uA796\uA798\uA79A\uA79C\uA79E\uA7A0\uA7A2\uA7A4"),
      u51(u"\uA7A6\uA7A8\uA7AA\uA7AB\uA7AC\uA7AD\uA7B0\uA7B1\uFF21\uFF22\uFF23\uFF24\uFF25\uFF26\uFF27\uFF28\uFF29\uFF2A\uFF2B\uFF2C\uFF2D"),
      u52(u"\uFF2E\uFF2F\uFF30\uFF31\uFF32\uFF33\uFF34\uFF35\uFF36\uFF37\uFF38\uFF39\uFF3A\uFE5A\u0601\u0702\u0803\u0904\u0a05\u0b06\u0c07"),
      u53(u"\u0d08\u0e09\u0f0a\u3001\u4002\u5003\u6004\u7005\u8006\u9007\ub008\uc009\ud00a\ue00b");
    ucs2String
      l00(u"\u0061\u0062\u0063\u0064\u0065\u0066\u0067\u0068\u0069\u006A\u006B\u006C\u006D\u006E\u006F\u0070\u0071\u0072\u0073\u0074\u0075"),
      l01(u"\u0076\u0077\u0078\u0079\u007A\u00E0\u00E1\u00E2\u00E3\u00E4\u00E5\u00E6\u00E7\u00E8\u00E9\u00EA\u00EB\u00EC\u00ED\u00EE\u00EF"),
      l02(u"\u00F0\u00F1\u00F2\u00F3\u00F4\u00F5\u00F6\u00F8\u00F9\u00FA\u00FB\u00FC\u00FD\u00FE\u0101\u0105\u0107\u0109\u010B\u010D\u010F"),
      l03(u"\u0111\u0113\u0115\u0117\u0119\u011B\u011D\u011F\u0121\u0123\u0125\u0127\u0129\u012B\u012D\u012F\u0069\u0133\u0135\u0137\u013A"),
      l04(u"\u013C\u013E\u0140\u0142\u0144\u0146\u0148\u014B\u014D\u014F\u0151\u0153\u0155\u0157\u0159\u015B\u015D\u015F\u0161\u0163\u0165"),
      l05(u"\u0167\u0169\u016B\u016D\u016F\u0171\u0173\u0175\u0177\u00FF\u017A\u017C\u017E\u0253\u0183\u0185\u0254\u0188\u0256\u0257\u018C"),
      l06(u"\u01DD\u0259\u025B\u0192\u0260\u0263\u0269\u0268\u0199\u026F\u0272\u0275\u01A1\u01A3\u01A5\u0280\u01A8\u0283\u01AD\u0288\u01B0"),
      l07(u"\u028A\u028B\u01B4\u01B6\u0292\u01B9\u01BD\u01C6\u01C6\u01C9\u01C9\u01CC\u01CC\u01CE\u01D0\u01D2\u01D4\u01D6\u01D8\u01DA\u01DC"),
      l08(u"\u01DF\u01E1\u01E3\u01E5\u01E7\u01E9\u01EB\u01ED\u01EF\u01F3\u01F3\u01F5\u0195\u01BF\u01F9\u01FB\u01FD\u01FF\u0201\u0203\u0205"),
      l09(u"\u0207\u0209\u020B\u020D\u020F\u0211\u0213\u0215\u0217\u0219\u021B\u021D\u021F\u019E\u0223\u0225\u0227\u0229\u022B\u022D\u022F"),
      l10(u"\u0231\u0233\u2C65\u023C\u019A\u2C66\u0242\u0180\u0289\u028C\u0247\u0249\u024B\u024D\u024F\u0371\u0373\u0377\u03F3\u03AC\u03AD"),
      l11(u"\u03AE\u03AF\u03CC\u03CD\u03CE\u03B1\u03B2\u03B3\u03B4\u03B5\u03B6\u03B7\u03B8\u03B9\u03BA\u03BB\u03BC\u03BD\u03BE\u03BF\u03C0"),
      l12(u"\u03C1\u03C3\u03C4\u03C5\u03C6\u03C7\u03C8\u03C9\u03CA\u03CB\u03D7\u03D9\u03DB\u03DD\u03DF\u03E1\u03E3\u03E5\u03E7\u03E9\u03EB"),
      l13(u"\u03ED\u03EF\u03B8\u03F8\u03F2\u03FB\u037B\u037C\u037D\u0450\u0451\u0452\u0453\u0454\u0455\u0456\u0457\u0458\u0459\u045A\u045B"),
      l14(u"\u045C\u045D\u045E\u045F\u0430\u0431\u0432\u0433\u0434\u0435\u0436\u0437\u0438\u0439\u043A\u043B\u043C\u043D\u043E\u043F\u0440"),
      l15(u"\u0441\u0442\u0443\u0444\u0445\u0446\u0447\u0448\u0449\u044A\u044B\u044C\u044D\u044E\u044F\u0461\u0463\u0465\u0467\u0469\u046B"),
      l16(u"\u046D\u046F\u0471\u0473\u0475\u0477\u0479\u047B\u047D\u047F\u0481\u048B\u048D\u048F\u0491\u0493\u0495\u0497\u0499\u049B\u049D"),
      l17(u"\u049F\u04A1\u04A3\u04A5\u04A7\u04A9\u04AB\u04AD\u04AF\u04B1\u04B3\u04B5\u04B7\u04B9\u04BB\u04BD\u04BF\u04CF\u04C2\u04C4\u04C6"),
      l18(u"\u04C8\u04CA\u04CC\u04CE\u04D1\u04D3\u04D5\u04D7\u04D9\u04DB\u04DD\u04DF\u04E1\u04E3\u04E5\u04E7\u04E9\u04EB\u04ED\u04EF\u04F1"),
      l19(u"\u04F3\u04F5\u04F7\u04F9\u04FB\u04FD\u04FF\u0501\u0503\u0505\u0507\u0509\u050B\u050D\u050F\u0511\u0513\u0515\u0517\u0519\u051B"),
      l20(u"\u051D\u051F\u0521\u0523\u0525\u0527\u0529\u052B\u052D\u052F\u0561\u0562\u0563\u0564\u0565\u0566\u0567\u0568\u0569\u056A\u056B"),
      l21(u"\u056C\u056D\u056E\u056F\u0570\u0571\u0572\u0573\u0574\u0575\u0576\u0577\u0578\u0579\u057A\u057B\u057C\u057D\u057E\u057F\u0580"),
      l22(u"\u0581\u0582\u0583\u0584\u0585\u0586\u2D00\u2D01\u2D02\u2D03\u2D04\u2D05\u2D06\u2D07\u2D08\u2D09\u2D0A\u2D0B\u2D0C\u2D0D\u2D0E"),
      l23(u"\u2D0F\u2D10\u2D11\u2D12\u2D13\u2D14\u2D15\u2D16\u2D17\u2D18\u2D19\u2D1A\u2D1B\u2D1C\u2D1D\u2D1E\u2D1F\u2D20\u2D21\u2D22\u2D23"),
      l24(u"\u2D24\u2D25\u2D27\u2D2D\u1E01\u1E03\u1E05\u1E07\u1E09\u1E0B\u1E0D\u1E0F\u1E11\u1E13\u1E15\u1E17\u1E19\u1E1B\u1E1D"),
      l29(u"\u1E1F\u1E21\u1E23\u1E25\u1E27\u1E29\u1E2B\u1E2D\u1E2F\u1E31\u1E33\u1E35\u1E37\u1E39\u1E3B\u1E3D\u1E3F\u1E41\u1E43\u1E45\u1E47"),
      l30(u"\u1E49\u1E4B\u1E4D\u1E4F\u1E51\u1E53\u1E55\u1E57\u1E59\u1E5B\u1E5D\u1E5F\u1E61\u1E63\u1E65\u1E67\u1E69\u1E6B\u1E6D\u1E6F\u1E71"),
      l31(u"\u1E73\u1E75\u1E77\u1E79\u1E7B\u1E7D\u1E7F\u1E81\u1E83\u1E85\u1E87\u1E89\u1E8B\u1E8D\u1E8F\u1E91\u1E93\u1E95\u00DF\u1EA1\u1EA3"),
      l32(u"\u1EA5\u1EA7\u1EA9\u1EAB\u1EAD\u1EAF\u1EB1\u1EB3\u1EB5\u1EB7\u1EB9\u1EBB\u1EBD\u1EBF\u1EC1\u1EC3\u1EC5\u1EC7\u1EC9\u1ECB\u1ECD"),
      l33(u"\u1ECF\u1ED1\u1ED3\u1ED5\u1ED7\u1ED9\u1EDB\u1EDD\u1EDF\u1EE1\u1EE3\u1EE5\u1EE7\u1EE9\u1EEB\u1EED\u1EEF\u1EF1\u1EF3\u1EF5\u1EF7"),
      l34(u"\u1EF9\u1EFB\u1EFD\u1EFF\u1F00\u1F01\u1F02\u1F03\u1F04\u1F05\u1F06\u1F07\u1F10\u1F11\u1F12\u1F13\u1F14\u1F15\u1F20\u1F21\u1F22"),
      l35(u"\u1F23\u1F24\u1F25\u1F26\u1F27\u1F30\u1F31\u1F32\u1F33\u1F34\u1F35\u1F36\u1F37\u1F40\u1F41\u1F42\u1F43\u1F44\u1F45\u1F51\u1F53"),
      l36(u"\u1F55\u1F57\u1F60\u1F61\u1F62\u1F63\u1F64\u1F65\u1F66\u1F67\u1F80\u1F81\u1F82\u1F83\u1F84\u1F85\u1F86\u1F87\u1F90\u1F91\u1F92"),
      l37(u"\u1F93\u1F94\u1F95\u1F96\u1F97\u1FA0\u1FA1\u1FA2\u1FA3\u1FA4\u1FA5\u1FA6\u1FA7\u1FB0\u1FB1\u1F70\u1F71\u1FB3\u1F72\u1F73\u1F74"),
      l38(u"\u1F75\u1FC3\u1FD0\u1FD1\u1F76\u1F77\u1FE0\u1FE1\u1F7A\u1F7B\u1FE5\u1F78\u1F79\u1F7C\u1F7D\u1FF3\u03C9\u006B\u00E5\u214E\u2170"),
      l39(u"\u2171\u2172\u2173\u2174\u2175\u2176\u2177\u2178\u2179\u217A\u217B\u217C\u217D\u217E\u217F\u2184\u24D0\u24D1\u24D2\u24D3\u24D4"),
      l40(u"\u24D5\u24D6\u24D7\u24D8\u24D9\u24DA\u24DB\u24DC\u24DD\u24DE\u24DF\u24E0\u24E1\u24E2\u24E3\u24E4\u24E5\u24E6\u24E7\u24E8\u24E9"),
      l41(u"\u2C30\u2C31\u2C32\u2C33\u2C34\u2C35\u2C36\u2C37\u2C38\u2C39\u2C3A\u2C3B\u2C3C\u2C3D\u2C3E\u2C3F\u2C40\u2C41\u2C42\u2C43\u2C44"),
      l42(u"\u2C45\u2C46\u2C47\u2C48\u2C49\u2C4A\u2C4B\u2C4C\u2C4D\u2C4E\u2C4F\u2C50\u2C51\u2C52\u2C53\u2C54\u2C55\u2C56\u2C57\u2C58\u2C59"),
      l43(u"\u2C5A\u2C5B\u2C5C\u2C5D\u2C5E\u2C61\u026B\u1D7D\u027D\u2C68\u2C6A\u2C6C\u0251\u0271\u0250\u0252\u2C73\u2C76\u023F\u0240\u2C81"),
      l44(u"\u2C83\u2C85\u2C87\u2C89\u2C8B\u2C8D\u2C8F\u2C91\u2C93\u2C95\u2C97\u2C99\u2C9B\u2C9D\u2C9F\u2CA1\u2CA3\u2CA5\u2CA7\u2CA9\u2CAB"),
      l45(u"\u2CAD\u2CAF\u2CB1\u2CB3\u2CB5\u2CB7\u2CB9\u2CBB\u2CBD\u2CBF\u2CC1\u2CC3\u2CC5\u2CC7\u2CC9\u2CCB\u2CCD\u2CCF\u2CD1\u2CD3\u2CD5"),
      l46(u"\u2CD7\u2CD9\u2CDB\u2CDD\u2CDF\u2CE1\u2CE3\u2CEC\u2CEE\u2CF3\uA641\uA643\uA645\uA647\uA649\uA64B\uA64D\uA64F\uA651\uA653\uA655"),
      l47(u"\uA657\uA659\uA65B\uA65D\uA65F\uA661\uA663\uA665\uA667\uA669\uA66B\uA66D\uA681\uA683\uA685\uA687\uA689\uA68B\uA68D\uA68F\uA691"),
      l48(u"\uA693\uA695\uA697\uA699\uA69B\uA723\uA725\uA727\uA729\uA72B\uA72D\uA72F\uA733\uA735\uA737\uA739\uA73B\uA73D\uA73F\uA741\uA743"),
      l49(u"\uA745\uA747\uA749\uA74B\uA74D\uA74F\uA751\uA753\uA755\uA757\uA759\uA75B\uA75D\uA75F\uA761\uA763\uA765\uA767\uA769\uA76B\uA76D"),
      l50(u"\uA76F\uA77A\uA77C\u1D79\uA77F\uA781\uA783\uA785\uA787\uA78C\u0265\uA791\uA793\uA797\uA799\uA79B\uA79D\uA79F\uA7A1\uA7A3\uA7A5"),
      l51(u"\uA7A7\uA7A9\u0266\u025C\u0261\u026C\u029E\u0287\uFF41\uFF42\uFF43\uFF44\uFF45\uFF46\uFF47\uFF48\uFF49\uFF4A\uFF4B\uFF4C\uFF4D"),
      l52(u"\uFF4E\uFF4F\uFF50\uFF51\uFF52\uFF53\uFF54\uFF55\uFF56\uFF57\uFF58\uFF59\uFF5A\uFE5A\u0601\u0702\u0803\u0904\u0a05\u0b06\u0c07"),
      l53(u"\u0d08\u0e09\u0f0a\u3001\u4002\u5003\u6004\u7005\u8006\u9007\ub008\uc009\ud00a\ue00b");

    int64_t x = 0;
    x|= (l00 == toLower(u00)) ? 0b1 : 0;    // good
    x|= (l01 == toLower(u01)) ? 0b10 : 0;   // good
    x|= (l02 == toLower(u02)) ? 0b100 : 0;
    x|= (l03 == toLower(u03)) ? 0b1000 : 0;
    x|= (l04 == toLower(u04)) ? 0b10000 : 0;    // good
    x|= (l05 == toLower(u05)) ? 0b100000 : 0;   // good
    x|= (l06 == toLower(u06)) ? 0b1000000 : 0;  // good
    x|= (l07 == toLower(u07)) ? 0b10000000 : 0; // good
    x|= (l08 == toLower(u08)) ? 0b100000000 : 0;    // good
    x|= (l09 == toLower(u09)) ? 0b1000000000 : 0;   // good
    x|= (l10 == toLower(u10)) ? 0b10000000000 : 0;  // good
    x|= (l11 == toLower(u11)) ? 0b100000000000 : 0; // good
    x|= (l12 == toLower(u12)) ? 0b1000000000000 : 0;    // good
    x|= (l13 == toLower(u13)) ? 0b10000000000000 : 0;   // good
    x|= (l14 == toLower(u14)) ? 0b100000000000000 : 0;  // good
    x|= (l15 == toLower(u15)) ? 0b1000000000000000 : 0; // good
    //*************************************************
    x|= (l16 == toLower(u16)) ? 0b10000000000000000 : 0;    // good
    x|= (l17 == toLower(u17)) ? 0b100000000000000000 : 0;   // good
    x|= (l18 == toLower(u18)) ? 0b1000000000000000000 : 0;  // good
    x|= (l19 == toLower(u19)) ? 0b10000000000000000000 : 0; // good
    x|= (l20 == toLower(u20)) ? 0b100000000000000000000 : 0;
    x|= (l21 == toLower(u21)) ? 0b1000000000000000000000 : 0;   // good
    x|= (l22 == toLower(u22)) ? 0b10000000000000000000000 : 0;
    x|= (l23 == toLower(u23)) ? 0b100000000000000000000000 : 0;
    x|= (l24 == toLower(u24)) ? 0b1000000000000000000000000 : 0;
    x|=                         0b10000000000000000000000000    ;   // good
    x|=                         0b100000000000000000000000000    ;  // good
    x|=                         0b1000000000000000000000000000    ;
    x|=                         0b10000000000000000000000000000    ;    // good
    x|= (l29 == toLower(u29)) ? 0b100000000000000000000000000000 : 0;   // good
    x|= (l30 == toLower(u30)) ? 0b1000000000000000000000000000000 : 0;  // good
    x|= (l31 == toLower(u31)) ? 0b10000000000000000000000000000000 : 0; // good
    //*************************************************
    x|= (l32 == toLower(u32)) ? 0b100000000000000000000000000000000 : 0;    // good
    x|= (l33 == toLower(u33)) ? 0b1000000000000000000000000000000000 : 0;   // good
    x|= (l34 == toLower(u34)) ? 0b10000000000000000000000000000000000 : 0;  // good
    x|= (l35 == toLower(u35)) ? 0b100000000000000000000000000000000000 : 0; // good
    x|= (l36 == toLower(u36)) ? 0b1000000000000000000000000000000000000 : 0;
    x|= (l37 == toLower(u37)) ? 0b10000000000000000000000000000000000000 : 0;
    x|= (l38 == toLower(u38)) ? 0b100000000000000000000000000000000000000 : 0;
    x|= (l39 == toLower(u39)) ? 0b1000000000000000000000000000000000000000 : 0;
    x|= (l40 == toLower(u40)) ? 0b10000000000000000000000000000000000000000 : 0;
    x|= (l41 == toLower(u41)) ? 0b100000000000000000000000000000000000000000 : 0;
    x|= (l42 == toLower(u42)) ? 0b1000000000000000000000000000000000000000000 : 0;
    x|= (l43 == toLower(u43)) ? 0b10000000000000000000000000000000000000000000 : 0;
    x|= (l44 == toLower(u44)) ? 0b100000000000000000000000000000000000000000000 : 0;    // good
    x|= (l45 == toLower(u45)) ? 0b1000000000000000000000000000000000000000000000 : 0;   // good
    x|= (l46 == toLower(u46)) ? 0b10000000000000000000000000000000000000000000000 : 0;
    x|= (l47 == toLower(u47)) ? 0b100000000000000000000000000000000000000000000000 : 0; // good
    x|= (l48 == toLower(u48)) ? 0b1000000000000000000000000000000000000000000000000 : 0;
    //*************************************************
    x|= (l49 == toLower(u49)) ? 0b10000000000000000000000000000000000000000000000000 : 0;   // good
    x|= (l50 == toLower(u50)) ? 0b100000000000000000000000000000000000000000000000000 : 0;
    x|= (l51 == toLower(u51)) ? 0b1000000000000000000000000000000000000000000000000000 : 0;
    x|= (l52 == toLower(u52)) ? 0b10000000000000000000000000000000000000000000000000000 : 0;
    x|= (l53 == toLower(u53)) ? 0b100000000000000000000000000000000000000000000000000000 : 0;

    EXPECT_EQ( 0x3FFFFFFFFFFFFF, x );
    EXPECT_EQ(l00, toLower(l00));
    EXPECT_EQ(l01, toLower(l01));
    EXPECT_EQ(l02, toLower(l02));
    EXPECT_EQ(l03, toLower(l03));
    EXPECT_EQ(l04, toLower(l04));
    EXPECT_EQ(l05, toLower(l05));
    EXPECT_EQ(l06, toLower(l06));
    EXPECT_EQ(l07, toLower(l07));
    EXPECT_EQ(l08, toLower(l08));
    EXPECT_EQ(l09, toLower(l09));
    EXPECT_EQ(l10, toLower(l10));
    EXPECT_EQ(l11, toLower(l11));
    EXPECT_EQ(l12, toLower(l12));
    EXPECT_EQ(l13, toLower(l13));
    EXPECT_EQ(l14, toLower(l14));
    EXPECT_EQ(l15, toLower(l15));
    EXPECT_EQ(l16, toLower(l16));
    EXPECT_EQ(l17, toLower(l17));
    EXPECT_EQ(l18, toLower(l18));
    EXPECT_EQ(l19, toLower(l19));
    EXPECT_EQ(l20, toLower(l20));
    EXPECT_EQ(l21, toLower(l21));
    EXPECT_EQ(l22, toLower(l22));
    EXPECT_EQ(l23, toLower(l23));
    EXPECT_EQ(l24, toLower(l24));
    // EXPECT_EQ(l25, toLower(l25));
    // EXPECT_EQ(l26, toLower(l26));
    // EXPECT_EQ(l27, toLower(l27));
    // EXPECT_EQ(l28, toLower(l28));
    EXPECT_EQ(l29, toLower(l29));
    EXPECT_EQ(l30, toLower(l30));
    EXPECT_EQ(l31, toLower(l31));
    EXPECT_EQ(l32, toLower(l32));
    EXPECT_EQ(l33, toLower(l33));
    EXPECT_EQ(l34, toLower(l34));
    EXPECT_EQ(l35, toLower(l35));
    EXPECT_EQ(l36, toLower(l36));
    EXPECT_EQ(l37, toLower(l37));
    EXPECT_EQ(l38, toLower(l38));
    EXPECT_EQ(l39, toLower(l39));
    EXPECT_EQ(l40, toLower(l40));
    EXPECT_EQ(l41, toLower(l41));
    EXPECT_EQ(l42, toLower(l42));
    EXPECT_EQ(l43, toLower(l43));
    EXPECT_EQ(l44, toLower(l44));
    EXPECT_EQ(l45, toLower(l45));
    EXPECT_EQ(l46, toLower(l46));
    EXPECT_EQ(l47, toLower(l47));
    EXPECT_EQ(l48, toLower(l48));
    EXPECT_EQ(l49, toLower(l49));
    EXPECT_EQ(l50, toLower(l50));
    EXPECT_EQ(l51, toLower(l51));
    EXPECT_EQ(l52, toLower(l52));
    EXPECT_EQ(l53, toLower(l53));
}

TEST(StringToLowerTest, testThirtyTwoBitToLowers)
{
    ucs4String empty;
    EXPECT_TRUE(toLower(empty).empty());

    int64_t result =0;
    int64_t flag = 1;
    {
    {   // 0.00 - x
        ucs4String upperCase(U"\U00000041\U00000042\U00000043\U00000044\U00000045\U00000046\U00000047\U00000048\U00000049");
        ucs4String lowerCase(U"\U00000061\U00000062\U00000063\U00000064\U00000065\U00000066\U00000067\U00000068\U00000069");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.01 - x
        ucs4String upperCase(U"\U0000004A\U0000004B\U0000004C\U0000004D\U0000004E\U0000004F\U00000050\U00000051\U00000052");
        ucs4String lowerCase(U"\U0000006A\U0000006B\U0000006C\U0000006D\U0000006E\U0000006F\U00000070\U00000071\U00000072");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.02 - x
        ucs4String upperCase(U"\U00000053\U00000054\U00000055\U00000056\U00000057\U00000058\U00000059\U0000005A\U000000C0");
        ucs4String lowerCase(U"\U00000073\U00000074\U00000075\U00000076\U00000077\U00000078\U00000079\U0000007A\U000000E0");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.03 - x
        ucs4String upperCase(U"\U000000C1\U000000C2\U000000C3\U000000C4\U000000C5\U000000C6\U000000C7\U000000C8\U000000C9");
        ucs4String lowerCase(U"\U000000E1\U000000E2\U000000E3\U000000E4\U000000E5\U000000E6\U000000E7\U000000E8\U000000E9");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    {   // 0.04 - x
        ucs4String upperCase(U"\U000000CA\U000000CB\U000000CC\U000000CD\U000000CE\U000000CF\U000000D0\U000000D1\U000000D2");
        ucs4String lowerCase(U"\U000000EA\U000000EB\U000000EC\U000000ED\U000000EE\U000000EF\U000000F0\U000000F1\U000000F2");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.05 - x
        ucs4String upperCase(U"\U000000D3\U000000D4\U000000D5\U000000D6\U000000D8\U000000D9\U000000DA\U000000DB\U000000DC");
        ucs4String lowerCase(U"\U000000F3\U000000F4\U000000F5\U000000F6\U000000F8\U000000F9\U000000FA\U000000FB\U000000FC");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.06 - x
        ucs4String upperCase(U"\U000000DD\U000000DE\U00000100\U00000104\U00000106\U00000108\U0000010A\U0000010C\U0000010E");
        ucs4String lowerCase(U"\U000000FD\U000000FE\U00000101\U00000105\U00000107\U00000109\U0000010B\U0000010D\U0000010F");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.07 - x
        ucs4String upperCase(U"\U00000110\U00000112\U00000114\U00000116\U00000118\U0000011A\U0000011C\U0000011E\U00000120");
        ucs4String lowerCase(U"\U00000111\U00000113\U00000115\U00000117\U00000119\U0000011B\U0000011D\U0000011F\U00000121");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    ////////////////////////////////////////////////////////////

    {   // 0.08 - x
        ucs4String upperCase(U"\U00000122\U00000124\U00000126\U00000128\U0000012A\U0000012C\U0000012E\U00000130\U00000132");
        ucs4String lowerCase(U"\U00000123\U00000125\U00000127\U00000129\U0000012B\U0000012D\U0000012F\U00000069\U00000133");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.09 - x
        ucs4String upperCase(U"\U00000134\U00000136\U00000139\U0000013B\U0000013D\U0000013F\U00000141\U00000143\U00000145");
        ucs4String lowerCase(U"\U00000135\U00000137\U0000013A\U0000013C\U0000013E\U00000140\U00000142\U00000144\U00000146");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.10 - x
        ucs4String upperCase(U"\U00000147\U0000014A\U0000014C\U0000014E\U00000150\U00000152\U00000154\U00000156\U00000158");
        ucs4String lowerCase(U"\U00000148\U0000014B\U0000014D\U0000014F\U00000151\U00000153\U00000155\U00000157\U00000159");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.11 - x
        ucs4String upperCase(U"\U0000015A\U0000015C\U0000015E\U00000160\U00000162\U00000164\U00000166\U00000168\U0000016A");
        ucs4String lowerCase(U"\U0000015B\U0000015D\U0000015F\U00000161\U00000163\U00000165\U00000167\U00000169\U0000016B");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    {   // 0.12 - x
        ucs4String upperCase(U"\U0000016C\U0000016E\U00000170\U00000172\U00000174\U00000176\U00000178\U00000179\U0000017B");
        ucs4String lowerCase(U"\U0000016D\U0000016F\U00000171\U00000173\U00000175\U00000177\U000000FF\U0000017A\U0000017C");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.13 - x
        ucs4String upperCase(U"\U0000017D\U00000181\U00000182\U00000184\U00000186\U00000187\U00000189\U0000018A\U0000018B");
        ucs4String lowerCase(U"\U0000017E\U00000253\U00000183\U00000185\U00000254\U00000188\U00000256\U00000257\U0000018C");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
    }
    {   // 0.14 - x
        ucs4String upperCase(U"\U0000018E\U0000018F\U00000190\U00000191\U00000193\U00000194\U00000196\U00000197\U00000198");
        ucs4String lowerCase(U"\U000001DD\U00000259\U0000025B\U00000192\U00000260\U00000263\U00000269\U00000268\U00000199");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.15 - x
        ucs4String upperCase(U"\U0000019C\U0000019D\U0000019F\U000001A0\U000001A2\U000001A4\U000001A6\U000001A7\U000001A9");
        ucs4String lowerCase(U"\U0000026F\U00000272\U00000275\U000001A1\U000001A3\U000001A5\U00000280\U000001A8\U00000283");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    ////////////////////////////////////////////////////////////

    {   // 0.16 - x
        ucs4String upperCase(U"\U000001AC\U000001AE\U000001AF\U000001B1\U000001B2\U000001B3\U000001B5\U000001B7\U000001B8");
        ucs4String lowerCase(U"\U000001AD\U00000288\U000001B0\U0000028A\U0000028B\U000001B4\U000001B6\U00000292\U000001B9");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.17 - x
        ucs4String upperCase(U"\U000001BC\U000001C4\U000001C5\U000001C7\U000001C8\U000001CA\U000001CB\U000001CD\U000001CF");
        ucs4String lowerCase(U"\U000001BD\U000001C6\U000001C6\U000001C9\U000001C9\U000001CC\U000001CC\U000001CE\U000001D0");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.18 - x
        ucs4String upperCase(U"\U000001D1\U000001D3\U000001D5\U000001D7\U000001D9\U000001DB\U000001DE\U000001E0\U000001E2");
        ucs4String lowerCase(U"\U000001D2\U000001D4\U000001D6\U000001D8\U000001DA\U000001DC\U000001DF\U000001E1\U000001E3");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.19 - x
        ucs4String upperCase(U"\U000001E4\U000001E6\U000001E8\U000001EA\U000001EC\U000001EE\U000001F1\U000001F2\U000001F4");
        ucs4String lowerCase(U"\U000001E5\U000001E7\U000001E9\U000001EB\U000001ED\U000001EF\U000001F3\U000001F3\U000001F5");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    {   // 0.20 - x
        ucs4String upperCase(U"\U000001F6\U000001F7\U000001F8\U000001FA\U000001FC\U000001FE\U00000200\U00000202\U00000204");
        ucs4String lowerCase(U"\U00000195\U000001BF\U000001F9\U000001FB\U000001FD\U000001FF\U00000201\U00000203\U00000205");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.21 - x
        ucs4String upperCase(U"\U00000206\U00000208\U0000020A\U0000020C\U0000020E\U00000210\U00000212\U00000214\U00000216");
        ucs4String lowerCase(U"\U00000207\U00000209\U0000020B\U0000020D\U0000020F\U00000211\U00000213\U00000215\U00000217");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.22 - x
        ucs4String upperCase(U"\U00000218\U0000021A\U0000021C\U0000021E\U00000220\U00000222\U00000224\U00000226\U00000228");
        ucs4String lowerCase(U"\U00000219\U0000021B\U0000021D\U0000021F\U0000019E\U00000223\U00000225\U00000227\U00000229");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.23 - x
        ucs4String upperCase(U"\U0000022A\U0000022C\U0000022E\U00000230\U00000232\U0000023A\U0000023B\U0000023D\U0000023E");
        ucs4String lowerCase(U"\U0000022B\U0000022D\U0000022F\U00000231\U00000233\U00002C65\U0000023C\U0000019A\U00002C66");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    ////////////////////////////////////////////////////////////

    {   // 0.24 - x
        ucs4String upperCase(U"\U00000241\U00000243\U00000244\U00000245\U00000246\U00000248\U0000024A\U0000024C\U0000024E");
        ucs4String lowerCase(U"\U00000242\U00000180\U00000289\U0000028C\U00000247\U00000249\U0000024B\U0000024D\U0000024F");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.25 - x
        ucs4String upperCase(U"\U00000370\U00000372\U00000376\U0000037F\U00000386\U00000388\U00000389\U0000038A\U0000038C");
        ucs4String lowerCase(U"\U00000371\U00000373\U00000377\U000003F3\U000003AC\U000003AD\U000003AE\U000003AF\U000003CC");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.26 - x
        ucs4String upperCase(U"\U0000038E\U0000038F\U00000391\U00000392\U00000393\U00000394\U00000395\U00000396\U00000397");
        ucs4String lowerCase(U"\U000003CD\U000003CE\U000003B1\U000003B2\U000003B3\U000003B4\U000003B5\U000003B6\U000003B7");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.27 - x
        ucs4String upperCase(U"\U00000398\U00000399\U0000039A\U0000039B\U0000039C\U0000039D\U0000039E\U0000039F\U000003A0");
        ucs4String lowerCase(U"\U000003B8\U000003B9\U000003BA\U000003BB\U000003BC\U000003BD\U000003BE\U000003BF\U000003C0");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.28 - x
        ucs4String upperCase(U"\U000003A1\U000003A3\U000003A4\U000003A5\U000003A6\U000003A7\U000003A8\U000003A9\U000003AA");
        ucs4String lowerCase(U"\U000003C1\U000003C3\U000003C4\U000003C5\U000003C6\U000003C7\U000003C8\U000003C9\U000003CA");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.29 - x
        ucs4String upperCase(U"\U000003AB\U000003CF\U000003D8\U000003DA\U000003DC\U000003DE\U000003E0\U000003E2\U000003E4");
        ucs4String lowerCase(U"\U000003CB\U000003D7\U000003D9\U000003DB\U000003DD\U000003DF\U000003E1\U000003E3\U000003E5");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.30 - x
        ucs4String upperCase(U"\U000003E6\U000003E8\U000003EA\U000003EC\U000003EE\U000003F4\U000003F7\U000003F9\U000003FA");
        ucs4String lowerCase(U"\U000003E7\U000003E9\U000003EB\U000003ED\U000003EF\U000003B8\U000003F8\U000003F2\U000003FB");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.31 - x
        ucs4String upperCase(U"\U000003FD\U000003FE\U000003FF\U00000400\U00000401\U00000402\U00000403\U00000404\U00000405");
        ucs4String lowerCase(U"\U0000037B\U0000037C\U0000037D\U00000450\U00000451\U00000452\U00000453\U00000454\U00000455");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    ////////////////////////////////////////////////////////////

    {   // 0.32 - x
        ucs4String upperCase(U"\U00000406\U00000407\U00000408\U00000409\U0000040A\U0000040B\U0000040C\U0000040D\U0000040E");
        ucs4String lowerCase(U"\U00000456\U00000457\U00000458\U00000459\U0000045A\U0000045B\U0000045C\U0000045D\U0000045E");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.33 - x
        ucs4String upperCase(U"\U0000040F\U00000410\U00000411\U00000412\U00000413\U00000414\U00000415\U00000416\U00000417");
        ucs4String lowerCase(U"\U0000045F\U00000430\U00000431\U00000432\U00000433\U00000434\U00000435\U00000436\U00000437");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.34 - x
        ucs4String upperCase(U"\U00000418\U00000419\U0000041A\U0000041B\U0000041C\U0000041D\U0000041E\U0000041F\U00000420");
        ucs4String lowerCase(U"\U00000438\U00000439\U0000043A\U0000043B\U0000043C\U0000043D\U0000043E\U0000043F\U00000440");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.35 - x
        ucs4String upperCase(U"\U00000421\U00000422\U00000423\U00000424\U00000425\U00000426\U00000427\U00000428\U00000429");
        ucs4String lowerCase(U"\U00000441\U00000442\U00000443\U00000444\U00000445\U00000446\U00000447\U00000448\U00000449");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.36 - x
        ucs4String upperCase(U"\U0000042A\U0000042B\U0000042C\U0000042D\U0000042E\U0000042F\U00000460\U00000462\U00000464");
        ucs4String lowerCase(U"\U0000044A\U0000044B\U0000044C\U0000044D\U0000044E\U0000044F\U00000461\U00000463\U00000465");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.37 - x
        ucs4String upperCase(U"\U00000466\U00000468\U0000046A\U0000046C\U0000046E\U00000470\U00000472\U00000474\U00000476");
        ucs4String lowerCase(U"\U00000467\U00000469\U0000046B\U0000046D\U0000046F\U00000471\U00000473\U00000475\U00000477");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.38 - x
        ucs4String upperCase(U"\U00000478\U0000047A\U0000047C\U0000047E\U00000480\U0000048A\U0000048C\U0000048E\U00000490");
        ucs4String lowerCase(U"\U00000479\U0000047B\U0000047D\U0000047F\U00000481\U0000048B\U0000048D\U0000048F\U00000491");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.39 - x
        ucs4String upperCase(U"\U00000492\U00000494\U00000496\U00000498\U0000049A\U0000049C\U0000049E\U000004A0\U000004A2");
        ucs4String lowerCase(U"\U00000493\U00000495\U00000497\U00000499\U0000049B\U0000049D\U0000049F\U000004A1\U000004A3");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    ////////////////////////////////////////////////////////////

    {   // 0.40 - x
        ucs4String upperCase(U"\U000004A4\U000004A6\U000004A8\U000004AA\U000004AC\U000004AE\U000004B0\U000004B2\U000004B4");
        ucs4String lowerCase(U"\U000004A5\U000004A7\U000004A9\U000004AB\U000004AD\U000004AF\U000004B1\U000004B3\U000004B5");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.41 - x
        ucs4String upperCase(U"\U000004B6\U000004B8\U000004BA\U000004BC\U000004BE\U000004C0\U000004C1\U000004C3\U000004C5");
        ucs4String lowerCase(U"\U000004B7\U000004B9\U000004BB\U000004BD\U000004BF\U000004CF\U000004C2\U000004C4\U000004C6");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.42 - x
        ucs4String upperCase(U"\U000004C7\U000004C9\U000004CB\U000004CD\U000004D0\U000004D2\U000004D4\U000004D6\U000004D8");
        ucs4String lowerCase(U"\U000004C8\U000004CA\U000004CC\U000004CE\U000004D1\U000004D3\U000004D5\U000004D7\U000004D9");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.43 - x
        ucs4String upperCase(U"\U000004DA\U000004DC\U000004DE\U000004E0\U000004E2\U000004E4\U000004E6\U000004E8\U000004EA");
        ucs4String lowerCase(U"\U000004DB\U000004DD\U000004DF\U000004E1\U000004E3\U000004E5\U000004E7\U000004E9\U000004EB");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    {   // 0.44 - x
        ucs4String upperCase(U"\U000004EC\U000004EE\U000004F0\U000004F2\U000004F4\U000004F6\U000004F8\U000004FA\U000004FC");
        ucs4String lowerCase(U"\U000004ED\U000004EF\U000004F1\U000004F3\U000004F5\U000004F7\U000004F9\U000004FB\U000004FD");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.45 - x
        ucs4String upperCase(U"\U000004FE\U00000500\U00000502\U00000504\U00000506\U00000508\U0000050A\U0000050C\U0000050E");
        ucs4String lowerCase(U"\U000004FF\U00000501\U00000503\U00000505\U00000507\U00000509\U0000050B\U0000050D\U0000050F");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.46 - x
        ucs4String upperCase(U"\U00000510\U00000512\U00000514\U00000516\U00000518\U0000051A\U0000051C\U0000051E\U00000520");
        ucs4String lowerCase(U"\U00000511\U00000513\U00000515\U00000517\U00000519\U0000051B\U0000051D\U0000051F\U00000521");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.47 - x
        ucs4String upperCase(U"\U00000522\U00000524\U00000526\U00000528\U0000052A\U0000052C\U0000052E\U00000531\U00000532");
        ucs4String lowerCase(U"\U00000523\U00000525\U00000527\U00000529\U0000052B\U0000052D\U0000052F\U00000561\U00000562");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    ////////////////////////////////////////////////////////////

    {   // 0.48 - x
        ucs4String upperCase(U"\U00000533\U00000534\U00000535\U00000536\U00000537\U00000538\U00000539\U0000053A\U0000053B");
        ucs4String lowerCase(U"\U00000563\U00000564\U00000565\U00000566\U00000567\U00000568\U00000569\U0000056A\U0000056B");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.49 - x
        ucs4String upperCase(U"\U0000053C\U0000053D\U0000053E\U0000053F\U00000540\U00000541\U00000542\U00000543\U00000544");
        ucs4String lowerCase(U"\U0000056C\U0000056D\U0000056E\U0000056F\U00000570\U00000571\U00000572\U00000573\U00000574");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.50 - x
        ucs4String upperCase(U"\U00000545\U00000546\U00000547\U00000548\U00000549\U0000054A\U0000054B\U0000054C\U0000054D");
        ucs4String lowerCase(U"\U00000575\U00000576\U00000577\U00000578\U00000579\U0000057A\U0000057B\U0000057C\U0000057D");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.51 - x
        ucs4String upperCase(U"\U0000054E\U0000054F\U00000550\U00000551\U00000552\U00000553\U00000554\U00000555\U00000556");
        ucs4String lowerCase(U"\U0000057E\U0000057F\U00000580\U00000581\U00000582\U00000583\U00000584\U00000585\U00000586");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    {   // 0.52 - x
        ucs4String upperCase(U"\U000010A0\U000010A1\U000010A2\U000010A3\U000010A4\U000010A5\U000010A6\U000010A7\U000010A8");
        ucs4String lowerCase(U"\U00002D00\U00002D01\U00002D02\U00002D03\U00002D04\U00002D05\U00002D06\U00002D07\U00002D08");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.53 - x
        ucs4String upperCase(U"\U000010A9\U000010AA\U000010AB\U000010AC\U000010AD\U000010AE\U000010AF\U000010B0\U000010B1");
        ucs4String lowerCase(U"\U00002D09\U00002D0A\U00002D0B\U00002D0C\U00002D0D\U00002D0E\U00002D0F\U00002D10\U00002D11");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.54 - x
        ucs4String upperCase(U"\U000010B2\U000010B3\U000010B4\U000010B5\U000010B6\U000010B7\U000010B8\U000010B9\U000010BA");
        ucs4String lowerCase(U"\U00002D12\U00002D13\U00002D14\U00002D15\U00002D16\U00002D17\U00002D18\U00002D19\U00002D1A");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.55 - x
        ucs4String upperCase(U"\U000010BB\U000010BC\U000010BD\U000010BE\U000010BF\U000010C0\U000010C1\U000010C2\U000010C3");
        ucs4String lowerCase(U"\U00002D1B\U00002D1C\U00002D1D\U00002D1E\U00002D1F\U00002D20\U00002D21\U00002D22\U00002D23");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    ////////////////////////////////////////////////////////////

    {   // 0.56 - x
        ucs4String upperCase(U"\U000010C4\U000010C5\U000010C7\U000010CD");
        ucs4String lowerCase(U"\U00002D24\U00002D25\U00002D27\U00002D2D");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 0.57 - x
        result |= flag;
        flag <<= 1;
    }
    {   // 0.58 - x
        result |= flag;
        flag <<= 1;
    }
    {   // 0.59 - x
        result |= flag;
        flag <<= 1;
    }
    ////////////////////////////////////////////////////////////

    {   // 0.60 - x
        result |= flag;
        flag <<= 1;
    }
    {   // 0.61 - x
        result |= flag;
        flag <<= 1;
    }
    {   // 0.62 - x
        result |= flag;
        flag <<= 1;
    }
    }

    auto result0 = result; result = 0;
    flag = 1;
    // result0 should be 0x7FFFFFFFFFFFFFFF

    {
    ////////////////////////////////////////////////////////////
    {   // 1.00 - x
        result |= flag;
        flag <<= 1;
    }
    {   // 1.01 - x
        result |= flag;
        flag <<= 1;
    }
    {   // 1.02 - x
        result |= flag;
        flag <<= 1;
    }
    {   // 1.03 - x
        ucs4String upperCase(U"\U00001E00\U00001E02\U00001E04\U00001E06\U00001E08\U00001E0A\U00001E0C\U00001E0E\U00001E10");
        ucs4String lowerCase(U"\U00001E01\U00001E03\U00001E05\U00001E07\U00001E09\U00001E0B\U00001E0D\U00001E0F\U00001E11");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.04 - x
        ucs4String upperCase(U"\U00001E12\U00001E14\U00001E16\U00001E18\U00001E1A\U00001E1C\U00001E1E\U00001E20\U00001E22");
        ucs4String lowerCase(U"\U00001E13\U00001E15\U00001E17\U00001E19\U00001E1B\U00001E1D\U00001E1F\U00001E21\U00001E23");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.05 - x
        ucs4String upperCase(U"\U00001E24\U00001E26\U00001E28\U00001E2A\U00001E2C\U00001E2E\U00001E30\U00001E32\U00001E34");
        ucs4String lowerCase(U"\U00001E25\U00001E27\U00001E29\U00001E2B\U00001E2D\U00001E2F\U00001E31\U00001E33\U00001E35");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.06 - x
        ucs4String upperCase(U"\U00001E36\U00001E38\U00001E3A\U00001E3C\U00001E3E\U00001E40\U00001E42\U00001E44\U00001E46");
        ucs4String lowerCase(U"\U00001E37\U00001E39\U00001E3B\U00001E3D\U00001E3F\U00001E41\U00001E43\U00001E45\U00001E47");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.07 - x
        ucs4String upperCase(U"\U00001E48\U00001E4A\U00001E4C\U00001E4E\U00001E50\U00001E52\U00001E54\U00001E56\U00001E58");
        ucs4String lowerCase(U"\U00001E49\U00001E4B\U00001E4D\U00001E4F\U00001E51\U00001E53\U00001E55\U00001E57\U00001E59");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 1.08 - x
        ucs4String upperCase(U"\U00001E5A\U00001E5C\U00001E5E\U00001E60\U00001E62\U00001E64\U00001E66\U00001E68\U00001E6A");
        ucs4String lowerCase(U"\U00001E5B\U00001E5D\U00001E5F\U00001E61\U00001E63\U00001E65\U00001E67\U00001E69\U00001E6B");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.09 - x
        ucs4String upperCase(U"\U00001E6C\U00001E6E\U00001E70\U00001E72\U00001E74\U00001E76\U00001E78\U00001E7A\U00001E7C");
        ucs4String lowerCase(U"\U00001E6D\U00001E6F\U00001E71\U00001E73\U00001E75\U00001E77\U00001E79\U00001E7B\U00001E7D");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.10 - x
        ucs4String upperCase(U"\U00001E7E\U00001E80\U00001E82\U00001E84\U00001E86\U00001E88\U00001E8A\U00001E8C\U00001E8E");
        ucs4String lowerCase(U"\U00001E7F\U00001E81\U00001E83\U00001E85\U00001E87\U00001E89\U00001E8B\U00001E8D\U00001E8F");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.11 - x
        ucs4String upperCase(U"\U00001E90\U00001E92\U00001E94\U00001E9E\U00001EA0\U00001EA2\U00001EA4\U00001EA6\U00001EA8");
        ucs4String lowerCase(U"\U00001E91\U00001E93\U00001E95\U000000DF\U00001EA1\U00001EA3\U00001EA5\U00001EA7\U00001EA9");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.12 - x
        ucs4String upperCase(U"\U00001EAA\U00001EAC\U00001EAE\U00001EB0\U00001EB2\U00001EB4\U00001EB6\U00001EB8\U00001EBA");
        ucs4String lowerCase(U"\U00001EAB\U00001EAD\U00001EAF\U00001EB1\U00001EB3\U00001EB5\U00001EB7\U00001EB9\U00001EBB");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.13 - x
        ucs4String upperCase(U"\U00001EBC\U00001EBE\U00001EC0\U00001EC2\U00001EC4\U00001EC6\U00001EC8\U00001ECA\U00001ECC");
        ucs4String lowerCase(U"\U00001EBD\U00001EBF\U00001EC1\U00001EC3\U00001EC5\U00001EC7\U00001EC9\U00001ECB\U00001ECD");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.14 - x
        ucs4String upperCase(U"\U00001ECE\U00001ED0\U00001ED2\U00001ED4\U00001ED6\U00001ED8\U00001EDA\U00001EDC\U00001EDE");
        ucs4String lowerCase(U"\U00001ECF\U00001ED1\U00001ED3\U00001ED5\U00001ED7\U00001ED9\U00001EDB\U00001EDD\U00001EDF");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.15 - x
        ucs4String upperCase(U"\U00001EE0\U00001EE2\U00001EE4\U00001EE6\U00001EE8\U00001EEA\U00001EEC\U00001EEE\U00001EF0");
        ucs4String lowerCase(U"\U00001EE1\U00001EE3\U00001EE5\U00001EE7\U00001EE9\U00001EEB\U00001EED\U00001EEF\U00001EF1");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 1.16 - x
        ucs4String upperCase(U"\U00001EF2\U00001EF4\U00001EF6\U00001EF8\U00001EFA\U00001EFC\U00001EFE\U00001F08\U00001F09");
        ucs4String lowerCase(U"\U00001EF3\U00001EF5\U00001EF7\U00001EF9\U00001EFB\U00001EFD\U00001EFF\U00001F00\U00001F01");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.17 - x
        ucs4String upperCase(U"\U00001F0A\U00001F0B\U00001F0C\U00001F0D\U00001F0E\U00001F0F\U00001F18\U00001F19\U00001F1A");
        ucs4String lowerCase(U"\U00001F02\U00001F03\U00001F04\U00001F05\U00001F06\U00001F07\U00001F10\U00001F11\U00001F12");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.18 - x
        ucs4String upperCase(U"\U00001F1B\U00001F1C\U00001F1D\U00001F28\U00001F29\U00001F2A\U00001F2B\U00001F2C\U00001F2D");
        ucs4String lowerCase(U"\U00001F13\U00001F14\U00001F15\U00001F20\U00001F21\U00001F22\U00001F23\U00001F24\U00001F25");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.19 - x
        ucs4String upperCase(U"\U00001F2E\U00001F2F\U00001F38\U00001F39\U00001F3A\U00001F3B\U00001F3C\U00001F3D\U00001F3E");
        ucs4String lowerCase(U"\U00001F26\U00001F27\U00001F30\U00001F31\U00001F32\U00001F33\U00001F34\U00001F35\U00001F36");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.20 - x
        ucs4String upperCase(U"\U00001F3F\U00001F48\U00001F49\U00001F4A\U00001F4B\U00001F4C\U00001F4D\U00001F59\U00001F5B");
        ucs4String lowerCase(U"\U00001F37\U00001F40\U00001F41\U00001F42\U00001F43\U00001F44\U00001F45\U00001F51\U00001F53");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.21 - x
        ucs4String upperCase(U"\U00001F5D\U00001F5F\U00001F68\U00001F69\U00001F6A\U00001F6B\U00001F6C\U00001F6D\U00001F6E");
        ucs4String lowerCase(U"\U00001F55\U00001F57\U00001F60\U00001F61\U00001F62\U00001F63\U00001F64\U00001F65\U00001F66");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.22 - x
        ucs4String upperCase(U"\U00001F6F\U00001F88\U00001F89\U00001F8A\U00001F8B\U00001F8C\U00001F8D\U00001F8E\U00001F8F");
        ucs4String lowerCase(U"\U00001F67\U00001F80\U00001F81\U00001F82\U00001F83\U00001F84\U00001F85\U00001F86\U00001F87");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.23 - x
        ucs4String upperCase(U"\U00001F98\U00001F99\U00001F9A\U00001F9B\U00001F9C\U00001F9D\U00001F9E\U00001F9F\U00001FA8");
        ucs4String lowerCase(U"\U00001F90\U00001F91\U00001F92\U00001F93\U00001F94\U00001F95\U00001F96\U00001F97\U00001FA0");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 1.24 - x
        ucs4String upperCase(U"\U00001FA9\U00001FAA\U00001FAB\U00001FAC\U00001FAD\U00001FAE\U00001FAF\U00001FB8\U00001FB9");
        ucs4String lowerCase(U"\U00001FA1\U00001FA2\U00001FA3\U00001FA4\U00001FA5\U00001FA6\U00001FA7\U00001FB0\U00001FB1");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.25 - x
        ucs4String upperCase(U"\U00001FBA\U00001FBB\U00001FBC\U00001FC8\U00001FC9\U00001FCA\U00001FCB\U00001FCC\U00001FD8");
        ucs4String lowerCase(U"\U00001F70\U00001F71\U00001FB3\U00001F72\U00001F73\U00001F74\U00001F75\U00001FC3\U00001FD0");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.26 - x
        ucs4String upperCase(U"\U00001FD9\U00001FDA\U00001FDB\U00001FE8\U00001FE9\U00001FEA\U00001FEB\U00001FEC\U00001FF8");
        ucs4String lowerCase(U"\U00001FD1\U00001F76\U00001F77\U00001FE0\U00001FE1\U00001F7A\U00001F7B\U00001FE5\U00001F78");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.27 - x
        ucs4String upperCase(U"\U00001FF9\U00001FFA\U00001FFB\U00001FFC\U00002126\U0000212A\U0000212B\U00002132\U00002160");
        ucs4String lowerCase(U"\U00001F79\U00001F7C\U00001F7D\U00001FF3\U000003C9\U0000006B\U000000E5\U0000214E\U00002170");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.28 - x
        ucs4String upperCase(U"\U00002161\U00002162\U00002163\U00002164\U00002165\U00002166\U00002167\U00002168\U00002169");
        ucs4String lowerCase(U"\U00002171\U00002172\U00002173\U00002174\U00002175\U00002176\U00002177\U00002178\U00002179");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.29 - x
        ucs4String upperCase(U"\U0000216A\U0000216B\U0000216C\U0000216D\U0000216E\U0000216F\U00002183\U000024B6\U000024B7");
        ucs4String lowerCase(U"\U0000217A\U0000217B\U0000217C\U0000217D\U0000217E\U0000217F\U00002184\U000024D0\U000024D1");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.30 - x
        ucs4String upperCase(U"\U000024B8\U000024B9\U000024BA\U000024BB\U000024BC\U000024BD\U000024BE\U000024BF\U000024C0");
        ucs4String lowerCase(U"\U000024D2\U000024D3\U000024D4\U000024D5\U000024D6\U000024D7\U000024D8\U000024D9\U000024DA");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.31 - x
        ucs4String upperCase(U"\U000024C1\U000024C2\U000024C3\U000024C4\U000024C5\U000024C6\U000024C7\U000024C8\U000024C9");
        ucs4String lowerCase(U"\U000024DB\U000024DC\U000024DD\U000024DE\U000024DF\U000024E0\U000024E1\U000024E2\U000024E3");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 1.32 - x
        ucs4String upperCase(U"\U000024CA\U000024CB\U000024CC\U000024CD\U000024CE\U000024CF\U00002C00\U00002C01\U00002C02");
        ucs4String lowerCase(U"\U000024E4\U000024E5\U000024E6\U000024E7\U000024E8\U000024E9\U00002C30\U00002C31\U00002C32");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.33 - x
        ucs4String upperCase(U"\U00002C03\U00002C04\U00002C05\U00002C06\U00002C07\U00002C08\U00002C09\U00002C0A\U00002C0B");
        ucs4String lowerCase(U"\U00002C33\U00002C34\U00002C35\U00002C36\U00002C37\U00002C38\U00002C39\U00002C3A\U00002C3B");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.34 - x
        ucs4String upperCase(U"\U00002C0C\U00002C0D\U00002C0E\U00002C0F\U00002C10\U00002C11\U00002C12\U00002C13\U00002C14");
        ucs4String lowerCase(U"\U00002C3C\U00002C3D\U00002C3E\U00002C3F\U00002C40\U00002C41\U00002C42\U00002C43\U00002C44");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.35 - x
        ucs4String upperCase(U"\U00002C15\U00002C16\U00002C17\U00002C18\U00002C19\U00002C1A\U00002C1B\U00002C1C\U00002C1D");
        ucs4String lowerCase(U"\U00002C45\U00002C46\U00002C47\U00002C48\U00002C49\U00002C4A\U00002C4B\U00002C4C\U00002C4D");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.36 - x
        ucs4String upperCase(U"\U00002C1E\U00002C1F\U00002C20\U00002C21\U00002C22\U00002C23\U00002C24\U00002C25\U00002C26");
        ucs4String lowerCase(U"\U00002C4E\U00002C4F\U00002C50\U00002C51\U00002C52\U00002C53\U00002C54\U00002C55\U00002C56");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.37 - x
        ucs4String upperCase(U"\U00002C27\U00002C28\U00002C29\U00002C2A\U00002C2B\U00002C2C\U00002C2D\U00002C2E\U00002C60");
        ucs4String lowerCase(U"\U00002C57\U00002C58\U00002C59\U00002C5A\U00002C5B\U00002C5C\U00002C5D\U00002C5E\U00002C61");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.38 - x
        ucs4String upperCase(U"\U00002C62\U00002C63\U00002C64\U00002C67\U00002C69\U00002C6B\U00002C6D\U00002C6E\U00002C6F");
        ucs4String lowerCase(U"\U0000026B\U00001D7D\U0000027D\U00002C68\U00002C6A\U00002C6C\U00000251\U00000271\U00000250");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.39 - x
        ucs4String upperCase(U"\U00002C70\U00002C72\U00002C75\U00002C7E\U00002C7F\U00002C80\U00002C82\U00002C84\U00002C86");
        ucs4String lowerCase(U"\U00000252\U00002C73\U00002C76\U0000023F\U00000240\U00002C81\U00002C83\U00002C85\U00002C87");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 1.40 - x
        ucs4String upperCase(U"\U00002C88\U00002C8A\U00002C8C\U00002C8E\U00002C90\U00002C92\U00002C94\U00002C96\U00002C98");
        ucs4String lowerCase(U"\U00002C89\U00002C8B\U00002C8D\U00002C8F\U00002C91\U00002C93\U00002C95\U00002C97\U00002C99");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.41 - x
        ucs4String upperCase(U"\U00002C9A\U00002C9C\U00002C9E\U00002CA0\U00002CA2\U00002CA4\U00002CA6\U00002CA8\U00002CAA");
        ucs4String lowerCase(U"\U00002C9B\U00002C9D\U00002C9F\U00002CA1\U00002CA3\U00002CA5\U00002CA7\U00002CA9\U00002CAB");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.42 - x
        ucs4String upperCase(U"\U00002CAC\U00002CAE\U00002CB0\U00002CB2\U00002CB4\U00002CB6\U00002CB8\U00002CBA\U00002CBC");
        ucs4String lowerCase(U"\U00002CAD\U00002CAF\U00002CB1\U00002CB3\U00002CB5\U00002CB7\U00002CB9\U00002CBB\U00002CBD");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.43 - x
        ucs4String upperCase(U"\U00002CBE\U00002CC0\U00002CC2\U00002CC4\U00002CC6\U00002CC8\U00002CCA\U00002CCC\U00002CCE");
        ucs4String lowerCase(U"\U00002CBF\U00002CC1\U00002CC3\U00002CC5\U00002CC7\U00002CC9\U00002CCB\U00002CCD\U00002CCF");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.44 - x
        ucs4String upperCase(U"\U00002CD0\U00002CD2\U00002CD4\U00002CD6\U00002CD8\U00002CDA\U00002CDC\U00002CDE\U00002CE0");
        ucs4String lowerCase(U"\U00002CD1\U00002CD3\U00002CD5\U00002CD7\U00002CD9\U00002CDB\U00002CDD\U00002CDF\U00002CE1");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.45 - x
        ucs4String upperCase(U"\U00002CE2\U00002CEB\U00002CED\U00002CF2\U0000A640\U0000A642\U0000A644\U0000A646\U0000A648");
        ucs4String lowerCase(U"\U00002CE3\U00002CEC\U00002CEE\U00002CF3\U0000A641\U0000A643\U0000A645\U0000A647\U0000A649");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.46 - x
        ucs4String upperCase(U"\U0000A64A\U0000A64C\U0000A64E\U0000A650\U0000A652\U0000A654\U0000A656\U0000A658\U0000A65A");
        ucs4String lowerCase(U"\U0000A64B\U0000A64D\U0000A64F\U0000A651\U0000A653\U0000A655\U0000A657\U0000A659\U0000A65B");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.47 - x
        ucs4String upperCase(U"\U0000A65C\U0000A65E\U0000A660\U0000A662\U0000A664\U0000A666\U0000A668\U0000A66A\U0000A66C");
        ucs4String lowerCase(U"\U0000A65D\U0000A65F\U0000A661\U0000A663\U0000A665\U0000A667\U0000A669\U0000A66B\U0000A66D");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 1.48 - x
        ucs4String upperCase(U"\U0000A680\U0000A682\U0000A684\U0000A686\U0000A688\U0000A68A\U0000A68C\U0000A68E\U0000A690");
        ucs4String lowerCase(U"\U0000A681\U0000A683\U0000A685\U0000A687\U0000A689\U0000A68B\U0000A68D\U0000A68F\U0000A691");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.49 - x
        ucs4String upperCase(U"\U0000A692\U0000A694\U0000A696\U0000A698\U0000A69A\U0000A722\U0000A724\U0000A726\U0000A728");
        ucs4String lowerCase(U"\U0000A693\U0000A695\U0000A697\U0000A699\U0000A69B\U0000A723\U0000A725\U0000A727\U0000A729");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.50 - x
        ucs4String upperCase(U"\U0000A72A\U0000A72C\U0000A72E\U0000A732\U0000A734\U0000A736\U0000A738\U0000A73A\U0000A73C");
        ucs4String lowerCase(U"\U0000A72B\U0000A72D\U0000A72F\U0000A733\U0000A735\U0000A737\U0000A739\U0000A73B\U0000A73D");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.51 - x
        ucs4String upperCase(U"\U0000A73E\U0000A740\U0000A742\U0000A744\U0000A746\U0000A748\U0000A74A\U0000A74C\U0000A74E");
        ucs4String lowerCase(U"\U0000A73F\U0000A741\U0000A743\U0000A745\U0000A747\U0000A749\U0000A74B\U0000A74D\U0000A74F");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.52 - x
        ucs4String upperCase(U"\U0000A750\U0000A752\U0000A754\U0000A756\U0000A758\U0000A75A\U0000A75C\U0000A75E\U0000A760");
        ucs4String lowerCase(U"\U0000A751\U0000A753\U0000A755\U0000A757\U0000A759\U0000A75B\U0000A75D\U0000A75F\U0000A761");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.53 - x
        ucs4String upperCase(U"\U0000A762\U0000A764\U0000A766\U0000A768\U0000A76A\U0000A76C\U0000A76E\U0000A779\U0000A77B");
        ucs4String lowerCase(U"\U0000A763\U0000A765\U0000A767\U0000A769\U0000A76B\U0000A76D\U0000A76F\U0000A77A\U0000A77C");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.54 - x
        ucs4String upperCase(U"\U0000A77D\U0000A77E\U0000A780\U0000A782\U0000A784\U0000A786\U0000A78B\U0000A78D\U0000A790");
        ucs4String lowerCase(U"\U00001D79\U0000A77F\U0000A781\U0000A783\U0000A785\U0000A787\U0000A78C\U00000265\U0000A791");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.55 - x
        ucs4String upperCase(U"\U0000A792\U0000A796\U0000A798\U0000A79A\U0000A79C\U0000A79E\U0000A7A0\U0000A7A2\U0000A7A4");
        ucs4String lowerCase(U"\U0000A793\U0000A797\U0000A799\U0000A79B\U0000A79D\U0000A79F\U0000A7A1\U0000A7A3\U0000A7A5");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 1.56 - x
        ucs4String upperCase(U"\U0000A7A6\U0000A7A8\U0000A7AA\U0000A7AB\U0000A7AC\U0000A7AD\U0000A7B0\U0000A7B1\U0000FF21");
        ucs4String lowerCase(U"\U0000A7A7\U0000A7A9\U00000266\U0000025C\U00000261\U0000026C\U0000029E\U00000287\U0000FF41");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.57 - x
        ucs4String upperCase(U"\U0000FF22\U0000FF23\U0000FF24\U0000FF25\U0000FF26\U0000FF27\U0000FF28\U0000FF29\U0000FF2A");
        ucs4String lowerCase(U"\U0000FF42\U0000FF43\U0000FF44\U0000FF45\U0000FF46\U0000FF47\U0000FF48\U0000FF49\U0000FF4A");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.58 - x
        ucs4String upperCase(U"\U0000FF2B\U0000FF2C\U0000FF2D\U0000FF2E\U0000FF2F\U0000FF30\U0000FF31\U0000FF32\U0000FF33");
        ucs4String lowerCase(U"\U0000FF4B\U0000FF4C\U0000FF4D\U0000FF4E\U0000FF4F\U0000FF50\U0000FF51\U0000FF52\U0000FF53");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.59 - x
        ucs4String upperCase(U"\U0000FF34\U0000FF35\U0000FF36\U0000FF37\U0000FF38\U0000FF39\U0000FF3A\U00010400\U00010401");
        ucs4String lowerCase(U"\U0000FF54\U0000FF55\U0000FF56\U0000FF57\U0000FF58\U0000FF59\U0000FF5A\U00010428\U00010429");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.60 - x
        ucs4String upperCase(U"\U00010402\U00010403\U00010404\U00010405\U00010406\U00010407\U00010408\U00010409\U0001040A");
        ucs4String lowerCase(U"\U0001042A\U0001042B\U0001042C\U0001042D\U0001042E\U0001042F\U00010430\U00010431\U00010432");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.61 - x
        ucs4String upperCase(U"\U0001040B\U0001040C\U0001040D\U0001040E\U0001040F\U00010410\U00010411\U00010412\U00010413");
        ucs4String lowerCase(U"\U00010433\U00010434\U00010435\U00010436\U00010437\U00010438\U00010439\U0001043A\U0001043B");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 1.62 - x
        ucs4String upperCase(U"\U00010414\U00010415\U00010416\U00010417\U00010418");
        ucs4String lowerCase(U"\U0001043C\U0001043D\U0001043E\U0001043F\U00010440");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    }

    auto result1 = result; result = 0;
    flag = 1;
    // result1 should be 0x7FFFFFFFFFFFFFFF

    {
    {   // 2.00 - x
        ucs4String upperCase(U"\U00010419\U0001041A\U0001041B\U0001041C\U0001041D\U0001041E\U0001041F\U00010420\U00010421");
        ucs4String lowerCase(U"\U00010441\U00010442\U00010443\U00010444\U00010445\U00010446\U00010447\U00010448\U00010449");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.01 - x
        ucs4String upperCase(U"\U00010422\U00010423\U00010424\U00010425\U00010426\U00010427\U00010C80\U00010C81\U00010C82");
        ucs4String lowerCase(U"\U0001044A\U0001044B\U0001044C\U0001044D\U0001044E\U0001044F\U00010CC0\U00010CC1\U00010CC2");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.02 - x
        ucs4String upperCase(U"\U00010C83\U00010C84\U00010C85\U00010C86\U00010C87\U00010C88\U00010C89\U00010C8A\U00010C8B");
        ucs4String lowerCase(U"\U00010CC3\U00010CC4\U00010CC5\U00010CC6\U00010CC7\U00010CC8\U00010CC9\U00010CCA\U00010CCB");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.03 - x
        ucs4String upperCase(U"\U00010C8C\U00010C8D\U00010C8E\U00010C8F\U00010C90\U00010C91\U00010C92\U00010C93\U00010C94");
        ucs4String lowerCase(U"\U00010CCC\U00010CCD\U00010CCE\U00010CCF\U00010CD0\U00010CD1\U00010CD2\U00010CD3\U00010CD4");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 2.04 - x
        ucs4String upperCase(U"\U00010C95\U00010C96\U00010C97\U00010C98\U00010C99\U00010C9A\U00010C9B\U00010C9C\U00010C9D");
        ucs4String lowerCase(U"\U00010CD5\U00010CD6\U00010CD7\U00010CD8\U00010CD9\U00010CDA\U00010CDB\U00010CDC\U00010CDD");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.05 - x
        ucs4String upperCase(U"\U00010C9E\U00010C9F\U00010CA0\U00010CA1\U00010CA2\U00010CA3\U00010CA4\U00010CA5\U00010CA6");
        ucs4String lowerCase(U"\U00010CDE\U00010CDF\U00010CE0\U00010CE1\U00010CE2\U00010CE3\U00010CE4\U00010CE5\U00010CE6");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.06 - x
        ucs4String upperCase(U"\U00010CA7\U00010CA8\U00010CA9\U00010CAA\U00010CAB\U00010CAC\U00010CAD\U00010CAE\U00010CAF");
        ucs4String lowerCase(U"\U00010CE7\U00010CE8\U00010CE9\U00010CEA\U00010CEB\U00010CEC\U00010CED\U00010CEE\U00010CEF");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.07 - x
        ucs4String upperCase(U"\U00010CB0\U00010CB1\U00010CB2\U000118A0\U000118A1\U000118A2\U000118A3\U000118A4\U000118A5");
        ucs4String lowerCase(U"\U00010CF0\U00010CF1\U00010CF2\U000118C0\U000118C1\U000118C2\U000118C3\U000118C4\U000118C5");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }

    ////////////////////////////////////////////////////////////
    {   // 2.08 - x
        ucs4String upperCase(U"\U000118A6\U000118A7\U000118A8\U000118A9\U000118AA\U000118AB\U000118AC\U000118AD\U000118AE");
        ucs4String lowerCase(U"\U000118C6\U000118C7\U000118C8\U000118C9\U000118CA\U000118CB\U000118CC\U000118CD\U000118CE");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.09 - x
        ucs4String upperCase(U"\U000118AF\U000118B0\U000118B1\U000118B2\U000118B3\U000118B4\U000118B5\U000118B6\U000118B7");
        ucs4String lowerCase(U"\U000118CF\U000118D0\U000118D1\U000118D2\U000118D3\U000118D4\U000118D5\U000118D6\U000118D7");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    {   // 2.10 - x
        ucs4String upperCase(U"\U000118B8\U000118B9\U000118BA\U000118BB\U000118BC\U000118BD\U000118BE\U000118BF");
        ucs4String lowerCase(U"\U000118D8\U000118D9\U000118DA\U000118DB\U000118DC\U000118DD\U000118DE\U000118DF");
        result |= lowerCase == toLower(upperCase) ? flag : 0;
        flag <<= 1;
        EXPECT_EQ(lowerCase, toLower(lowerCase));
    }
    }

    auto result2 = result; result = 0;
    flag = 1;
    // result2 should be 0x7FF

    EXPECT_EQ( 0x7FFFFFFFFFFFFFFF, result0 );
    EXPECT_EQ( 0x7FFFFFFFFFFFFFFF, result1 );
    EXPECT_EQ( static_cast<int64_t>(0x7FF), result2 );
}

TEST(StringToLowerTest, testPageZeroUnshiftables)
{
    {
        string dontTouch00(u8"\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
                     u8"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
                     u8"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
                     u8"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F\x40"
                     u8"\x5C\x5D\x5E\x5F"
                     u8"\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"
                     u8"\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F");

        EXPECT_EQ(dontTouch00, toLower(dontTouch00));
    }

    {
        ucs2String dontTouch01(u"\u0138\u0149\u018d\u019b\u01aa\u01ab\u01ba\u01bb\u01be"
                               u"\u01c0\u01c1\u01c2\u01c3\u01f0");

        EXPECT_EQ(dontTouch01, toLower(dontTouch01));
    }

    {
        ucs2String dontTouch02(u"\u0221"
                               u"\u0234\u0235\u0236\u0237\u0238\u0239");

        EXPECT_EQ(dontTouch02, toLower(dontTouch02));
    }

    {
        ucs2String dontTouch03(u"\u0374\u0375\u0378\u0379\u037A\u037E"
                               u"\u0380\u0381\u0382\u0383\u0384\u0385\u0387\u038B\u038D"
                               u"\u0390\u03A2\u03B0\u03D2\u03D3\u03D4\u03F6\u03FC");

        EXPECT_EQ(dontTouch03, toLower(dontTouch03));
    }

    {
        ucs2String dontTouch04(u"\u0482\u0483\u0484\u0485\u0486\u0487\u0488\u0489\u04CF");

        EXPECT_EQ(dontTouch04, toLower(dontTouch04));
    }

    {
        ucs2String dontTouch05(u"\u0580\u0581\u0582\u0583\u0584\u0585\u0586\u0587"
                               u"\u0588\u0589\u058A\u058B\u058C\u058D\u058E\u058F"
                               u"\u0590\u0591\u0592\u0593\u0594\u0595\u0596\u0597"
                               u"\u0598\u0599\u059A\u059B\u059C\u059D\u059E\u059F");

        EXPECT_EQ(dontTouch05, toLower(dontTouch05));
    }
}

TEST(StringToLowerTest, testPageOneUnshiftables)
{
    ucs2String dontTouchP1(u"\u109f\u10c6\u10c8\u10c9\u10ca\u10cb\u10cc\u10ce\u10cf\u10d0"
                           u"\u139f\u13f6\u13f7\u13f8\u13f9\u13fa\u13fb\u13fc\u13fd\u13fe\u13ff\u1dff"
                           u"\u1ffd\u1ffe\u1fff"

    );

    EXPECT_EQ(dontTouchP1, toLower(dontTouchP1));
}

TEST(StringToLowerTest, testPageTwoUnshiftables)
{
    ucs2String dontTouchP2(u"\u215f\u2180\u2181\u2182\u2185\u2186\u2187\u2188\u2189"
                           u"\u24b0\u24b1\u24b2\u24b3\u24b4\u24b5"
                           u"\u24ea\u24eb\u24ec\u24ed\u24ee\u24ef\u2bfc\u2bfd\u2bfe\u2bff"
                           u"\u2c2f\u2c5f\u2c71\u2c74\u2c77\u2c78\u2c79\u2c7a\u2c7b\u2c7c\u2c7d"
                           u"\u2ce4\u2ce5\u2ce6\u2ce7\u2ce8\u2ce9\u2cea\u2cef\u2cf0\u2cf1"
                           u"\u2cf4\u2cf5\u2cf6\u2cf7\u2cf8\u2cf9\u2cfa\u2cfb\u2cfc\u2cfd\u2cfe\u2cff"
                           u"\u2d26\u2d28\u2d29\u2d2a\u2d2b\u2d2c\u2d2e\u2d2f\u2d30");
    EXPECT_EQ(dontTouchP2, toLower(dontTouchP2));
}

TEST(StringToLowerTest, testPageAToFUnshiftables)
{
    ucs2String dontTouchPAtoF(u"\ua5ff\ua60b\ua617\ua623\ua630\ua637\ua63c\ua63d\ua63e\ua63f"
                              u"\ua66e\ua66f\ua670\ua675\ua67a\ua67f\ua69c\ua69d\ua69e\ua69f"
                              u"\ua700\ua70c\ua712\ua718\ua71e\ua721\ua730\ua731"
                              u"\ua770\ua771\ua772\ua773\ua774\ua775\ua776\ua777\ua778"
                              u"\ua788\ua789\ua78a\ua78e\ua78f\ua794\ua795\ua7ae\ua7af"
                              u"\ua7b8\ua7b9\ua7ba\ua7bb\ua7bc\ua7bd\ua7be\ua7bf\ua7c0"
                              u"\uabc0\uabd0\uabe0\uabf0"
                              u"\uff00\uff04\uff08\uff0c\uff10\uff18\uff1c\uff1f\uff20"
                              u"\uff3b\uff3c\uff3d\uff3e\uff3f\uff40\uff5b\uff5c\uff5d\uff5e\uff5f\uff60");
    EXPECT_EQ(dontTouchPAtoF, toLower(dontTouchPAtoF));
}

TEST(StringToLowerTest, testPlane1Unshiftables)
{
    ucs4String dontTouchPlane1(U"\U000103fd\U000103fe\U000103ff\U00010450\U00010451\U00010452\U00010453"
                               U"\U00010c7d\U00010c7e\U00010c7f\U00010cb3\U00010cb4\U00010cb5\U00010cb6"
                               U"\U00010cbc\U00010cbd\U00010cbe\U00010cbf\U00010cf3\U00010cf4\U00010cf5"
                               U"\U0001189d\U0001189e\U0001189f\U000118e0\U000118e1\U000118e2\U000118e3");
    EXPECT_EQ(dontTouchPlane1, toLower(dontTouchPlane1));
}

TEST(StringToLowerTest, testCoverageGaps)
{
    ucs2String dontTouch1fNonUnicodes(u"\u1f1e\u1f1f\u1f4e\u1f4f\u1f58\u1f5a\u1f5c\u1f5e");
    EXPECT_EQ(dontTouch1fNonUnicodes, toLower(dontTouch1fNonUnicodes));
}
