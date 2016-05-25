ansak-string
============

Copyright 2015, Arthur N. Klassen; see LICENSE for usage guidelines (BSD 2-clause)

This library started as the first piece of a larger project that's gotten somewhat stalled. It's a simple string library.

It's all about purifying input. If your string (whether it's 7-bit ASCII, UTF-8, UTF-16, UCS-2 or UTF-32/UCS-4) makes it through my APIs (into 7-bit ASCII, UTF-8 etc.), then that's what it is. No more, no less. This includes filtering for publicly assigned Unicode values, currently following Unicode 8.0.

Commercial Support:

Available on request, subject to reasonable terms we can agree on. Please contact me via the address given on my github contact page.

Requests for more conversions from Windows Code Pages and/or ISO/ECMA 8859 encoding standards will be considered individually.

Additional stuff:

* unicodeLength gives number of UCS-4 codes (regardless of composite/composable points) in a string
* split and join, like python's string and array methods
* all-Unicode-sensitive "tolower". It can operate in "Turkic" mode if you pass in an (optional) constant C-string for a Turkic language's ISO-639 two or three character code. In this mode, I-dot and dotless-i are handled correctly for those languages.

ISO-639 codes for Turkic languages:

* Azerbaizhani: "az", "azb", "aze", "azj"
* Kazakh: "kk", "kaz"
* Tatar: "tt", "tat"
* Turkish "tr", "tur"

To come:

* normalizing to composed or to consitituent-parts strings?
* normalizing to older versions of Unicode?
* normalizing to newer versions of Unicode as they emerge.
