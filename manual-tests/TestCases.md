# Test cases for the manual testing

## Environment preparation

The files required for the tests are in the archive "manual-tests/TestRoot.zip". And there are following elements:

* folder **folder-with-big-file** with one big file to check that discovered matches are exposed in the real time without a delay till the end of the search process,
* folder **folder-without-permissions** with file but without permission to iterate it, in that way selecting this folder to search in causes the search failed,
* folder **non-ascii nämes**  with non-ASCII symbols in names and with whitespace in file names,
* folder **other-files** with ordinary files, one  **other-files/file-without-permissions.txt** has no permissions to read.


To prepare the environment to the test unpack that archive into any folder on the computer and make sure that  **folder-without-permissions** and file **other-files/file-without-permissions.txt** do not have permissions to read the content.

## Test cases

### Bad regular expression
Steps:
1. Enter regular expression "((Ha++"
2. Click on **Search** button and select folder **other-files**

Expected behavior:
1. Message "Search failed" is shown with the content "Permission denieMismatched '(' and ')' in regular expression" and nothing is outputted.


### The folder without permissions

Steps:
1. Enter regular expression ".+"
2. Click on **Search** button and select folder **folder-without-permissions**

Expected behavior:
1. Message "Search failed" is shown with the content "Permission denied" and nothing is outputted.

### Regular files
Steps:
1. Enter regular expression "H.+9"
2. Click on **Search** button and select folder **other-files**

Expected behavior:
1. Message "Finished" is shown with the content "Search finished successfully".
2. In output widget is the line that parsing file **other-files/file-without-permissions.txt** is failed.
3. For the file **other-files/small-file1.txt** there is the following content:
> Start file:/home/nikolai/tmp/TestRoot/other-files/small-file1.txt
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.080366 | 4RHQQKRLRHKQAFI1XC9M
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.080529 | HJC2K0WKNX3LNZEW61M9
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.080734 | APST3ALMKEHZ941KP09M
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.080919 | GA9BQC2XHTGB49M5AWBE
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.081055 | CDXQHOCKYLN0SURUC91X
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.081077 | HGW4YBX9V6FFKV0E1JJR
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.081123 | WT2C73HL51I9F68SNFYQ
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.081216 | OHI5Y5UF7H60W454JUL9
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.081251 | 1H36I518S9ECXEE2SSAD
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.081385 | ILIAL0CXHF37G9Y5YPDA
>   Automatically generated | small file to read from | 2025-06-04 11:52:03.081455 | HT7Z98MH808XWHNZUJLK
4. For the file **other-files/small-file2.txt** there is the following content:
> Start file:/home/nikolai/tmp/TestRoot/other-files/small-file2.txt
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309382 | 9XBHVMVY9AUB7OFS85GJ
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309578 | IXPDZLLHN3P9VUD354IQ
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309630 | GIYHEYK5L9QJ3VA4UIMI
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309762 | 6DHXN77IPQNN7SWA9PUW
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309793 | 0S39NY44C17LRDMFHG9I
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309864 | R2W7ECJD5JNH3QGZ9U0X
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309894 | XH7NKOI9259YJON73RYC
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.309904 | K9H5QVO0HYTLO92F7OH9
>   Automatically generated | small file to read from | 2025-06-04 11:52:08.310215 | H73JOR630TQ9CRW1EVH3
5. For the file **other-files/small-file3.txt** there is the following content:
> Start file:/home/nikolai/tmp/TestRoot/other-files/small-file3.txt
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.760893 | 2SHJR85AMA6UO9JUSMV3
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.760950 | TTEHBDYOIR99F2HH4SLO
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761126 | E162RTWKHJ95XJP3VG36
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761153 | VAHHQR31WCR4UOWO96PH
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761258 | 77H49PITA5HZPD253H9L
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761296 | XS4GVBFH4OHE6595AP6A
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761314 | XHO0FJMRC2T55H60Y6O9
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761351 | RTWUZT0HT87MM9DO2Z2T
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761483 | 3HQICF3I9G3FUZBG4N5S
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761539 | XL58Z6X9OQ9S2HZXJ29O
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761710 | PX5Y3XMPE9JHIUCLZX29
>   Automatically generated | small file to read from | 2025-06-04 11:52:10.761765 | 96A6HP1YRZO9ED8YGAGV

### Files with non-ASCII names.

Steps:
1. Enter regular expression "H..9"
2. Click on **Search** button and select folder **non-ascii nämes** 

Expected behavior:
1. Message "Finished" is shown with the content "Search finished successfully".
2. For the file **non-ascii nämes/file näme 1.txt** there is the following content:
> Start file:/home/nikolai/tmp/TestRoot/non-ascii nämes/file näme 1.txt
>   Automatically generated | file with non-ascii näme and cöntent ÜÜ | 2025-06-04 11:53:23.859835 | 45HWR9EV6C522A34KKPL
>   Automatically generated | file with non-ascii näme and cöntent ÜÜ | 2025-06-04 11:53:23.860339 | HL99P5L4WJES3UNH986G
3. For the file **non-ascii nämes/file näme 2.txt** there is the following content:
> Start file:/home/nikolai/tmp/TestRoot/non-ascii nämes/file näme 2.txt
>   Automatically generated | file with non-ascii näme and cöntent ÜÜ | 2025-06-04 11:53:28.026871 | L7ZHQO9A720RPESXNXPZ
>   Automatically generated | file with non-ascii näme and cöntent ÜÜ | 2025-06-04 11:53:28.027418 | KGZU7FN2SCVMLSQH7A9S
>   Automatically generated | file with non-ascii näme and cöntent ÜÜ | 2025-06-04 11:53:28.027719 | R7EXGIUHK29KML3KBEYQ

### Parsing big file

Steps:
1. Enter regular expression "H..9"
2. Click on **Search** button and select folder **folder-with-big-file**

Expected behavior:
1. Message "Finished" is shown with the content "Search finished successfully".
2. Found matches are outputted without the delay till the end of the search process.
3. The window reacts on clicks and might be maximized and minimized during the search process.