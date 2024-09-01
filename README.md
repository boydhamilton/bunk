
# bunk

# Overview
Bunk is a CLI to hide binaries in one another. It does so by 'bunking' the two [binaries](https://en.wikipedia.org/wiki/Binary_file) together, and seperating the bytes by a distinguishable signature so they are retrieveable even without a distinct start point. 

# Usage
1. Download `bunk.exe` from releases
2. Run `bunk.exe`. Be sure you move the files you wish to operate on into the same folder as Bunk prior to running operations.
3. Run a below command with required arguments. There will be a success or failure message at the end of excecution, and the failure message should have context above. Message me @boydhamilton on Discord with any errors you find to be unexpected or inexplicable.

# Commands
Bunk only supports two basic `hide` and `extract` functions.

### hide
`./bunk hide a.bin b.bin o.bin`<br>
Argument `a` such that `a` is any binary that will be hidden and encrypted. <br>
Argument `b` such that `b` is any binary that will be displayed or excecuted after the hide operations excecution. <br>
Argument `o` such that `o` is any binary, though ideally the same or similar format as `b`, and will be the out file that Bunk writes all data to.

### extract
`./bunk extract o.bin f.bin`<br>
Argument `o` such that `o` is a binary that has had the Bunk hide operation. <br>
Argument `f` such that `f` is any binary, though ideally the same or similar format as `a` (see above), and will be the out file to which Bunk writes the found and decrypted data of `a`.

# Security
On the most basic level, Bunk works by concatenating two binary files together. Due to the nature of most file formats, only the first binary file will display, leaving the second one 'hidden'. The two files are seperated by a distinguishable signature in order to extract the second file entirely and without any excess bytes that may have overspilt from the first. The probability of signature collision, the event that the signature sequence exists in the "public" binary is, for all intents and purposes, 0.

$\mathrm{P}(B \subseteq A) = (n-m)(\frac{1}{256})^{m} $

Where $B$ is the signature set of bytes, and $A$ is the file set of bytes and $n$ and $m$ are their respective lengths. $n$ will generally exist in the magnitude of over a million, and $m$ exists in the magnitude of 200-300. Taking $n = 1.6 * 10^{6}$ and $m=256$ this yields a collision probability of about

$\mathrm{P}(B \subseteq A) = \frac{1}{2^{2044}} * 10^{5} \approx 0 $

The basic concatenating two files with a signature, however, is breakable by a hex editor if one has ability to identity the binary file type based off of constants in heading. Bunk offsets this by encrypting the data using a [substitution cipher](https://en.wikipedia.org/wiki/Substitution_cipher), a one-to-one size encryption method. This cipher is breakable using frequency analysis, but will hold up to gentle examination.