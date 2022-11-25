#!/usr/bin/python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import os
import sys

# https://pl.wikipedia.org/wiki/ASCII
ascii = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
# https://en.wikipedia.org/wiki/Latin_Extended-A
latin_extended_a = "ĀāĂăĄąĆćĈĉĊċČčĎďĐđĒēĔĕĖėĘęĚěĜĝĞğĠġĢģĤĥĦħĨĩĪīĬĭĮįİıĲĳĴĵĶķĸĹĺĻļĽľĿŀŁłŃńŅņŇňŉŊŋŌōŎŏŐőŒœŔŕŖŗŘřŚśŜŝŞşŠšŢţŤťŦŧŨũŪūŬŭŮůŰűŲųŴŵŶŷŸŹźŻżŽžſ"
# https://en.wikipedia.org/wiki/Latin_Extended-B
latin_extended_b = "ƀƁƂƃƄƅƆƇƈƉƊƋƌƍƎƏƐƑƒƓƔƕƖƗƘƙƚƛƜƝƞƟƠơƢƣƤƥƦƧƨƩƪƫƬƭƮƯưƱƲƳƴƵƶƷƸƹƺƻƼƽƾƿǀǁǂǃǄǅǆǇǈǉǊǋǌǍǎǏǐǑǒǓǔǕǖǗǘǙǚǛǜǝǞǟǠǡǢǣǤǥǦǧǨǩǪǫǬǭǮǯǰǱǲǳǴǵǶǷǸǹǺǻǼǽǾǿȀȁȂȃȄȅȆȇȈȉȊȋȌȍȎȏȐȑȒȓȔȕȖȗȘșȚțȜȝȞȟȠȡȢȣȤȥȦȧȨȩȪȫȬȭȮȯȰȱȲȳȴȵȶȷȸȹȺȻȼȽȾȿɀɁɂɃɄɅɆɇɈɉɊɋɌɍɎɏ"
# https://en.wikipedia.org/wiki/Latin_Extended-C
latin_extended_c = "ⱠⱡⱢⱣⱤⱥⱦⱧⱨⱩⱪⱫⱬⱭⱮⱯⱰⱱⱲⱳⱴⱵⱶⱷⱸⱹⱺⱻⱼⱽⱾⱿ"

# https://www.gt-pressura.com/
gt_basic_latin = "ABCDEFGHIJKLMNNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
gt_extended_latin = "ÁĂÂÄẠÆǼÀĀĄÅÃĆČÇĈĊĎĐÉĔĚÊËĖẸÈĒĘẼĞǦĜĢĠĦĤÍĬÎÏİỊÌĲĪĮĨJĴĶĹĽĻĿŁŃŇŅƝÑŊÓŎÔÖỌŒÒŐŌǪØǾÕŔŘŖŚŠŞŜȘŦŤŢȚÞÚŬÛÜỤÙŰŪŲŮŨẂŴẄẀÝŶŸỲȲỸŹŽŻẞƏáăâäạæǽàāąåãćčçĉċďđéĕěêëėẹèēęẽğǧĝģġħĥíĭîïịıìĳīįĩjĵȷķĸĺľļŀłńŉňņɲñŋóŏôöọœòőōǫøǿõŕřŗśšşŝșŧťţțþúŭûüụùűūųůũẃŵẅẁýŷÿỳȳỹźžżßðə"
gt_symbols = "([{,;:.…-‘’“”‚„‹›·–—•«»)]}!¡?¿/'\"ʼ&@©℀℗℅®™¶§†‡*#№∅+−±×÷=°≈≠≥≤ℓ●○❶❷❸❹❺❻❼❽❾①②③④⑤⑥⑦⑧⑨$¢£¥€₿₩₫₮₴₸₹₺₽₿ƒ¤↑↗→↘↓↙←↖↔↕⇵↩↪↰↱↲↳↴⬐↺↻"

ignore = "\r\n\t"

if len(sys.argv) == 2 and sys.argv[1] == "--help":
    print("print_characters.py --help")
    print("    Prints help")
    print("print_characters.py")
    print("    Prints basic, latin and symbols characters from gt_pressura")
    print("print_characters.py FILES...")
    print("    FILES - e.g. directory/*.txt")
    exit()

chars = set()

if len(sys.argv) < 2:
    for c in (ascii + gt_basic_latin + gt_extended_latin + gt_symbols):
        chars.add(c)
else:
    for c in ascii:
        chars.add(c)

    for filename in sys.argv[1:]:
        f = open(filename)
        str = f.read()
        for i in range(len(str)):
            c = str[i]
            if ignore.find(c) == -1:
                chars.add(c)
        f.close()

for c in sorted(chars):
    print(c, end = '')
print("")
