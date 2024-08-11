
# bunk

# Overview
Bunk is a CLI to hide binaries in one another. It does so by 'bunking' the two binaries together, and seperating the bytes by a distinguishable signature so they are retrieveable even without a distinct start point. 

# Usage
1. Download `bunk.exe` from releases
2. Run `bunk.exe`. Be sure you move the files you wish to operate on into the same folder as Bunk prior to running operations.
3. Run a below command with required arguments. There will be a success or failure message at the end of excecution, and the failure message should have context above. Message me @boydhamilton on Discord with any errors you find to be unexpected or inexplicable.

# Commands
Right now, Bunk only supports two basic `hide` and `extract` functions.

### hide
`hide a.bin b.bin o.bin`<br>
Argument `a` such that `a` is any binary is the binary that will be hidden and encrypted. <br>
Argument `b` such that `b` is any binary that will be displayed or excecuted after the hide operations excecution. <br>
Argument `o` such that `o` is any binary, though ideally the same or similar format as `b`, and will be the out file that Bunk writes all data to.

### extract
`extract o.bin f.bin`<br>
Argument `o` such that `o` is a binary that has had the Bunk hide operation. <br>
Argument `f` such that `f` is any binary, though ideally the same or similar format as `a` (see above), and will be the out file to which Bunk writes the found and decrypted data of `a`.

# Security
On the most basic level, Bunk works by concatenating two binary files together. Due to the nature of most file formats, only the first binary file will display, leaving the second one 'hidden'. The two files are seperated by a distinguishable signature in order to extract the second file entirely and without any excess bytes that may have overspilt from the first. Bunk generally has a   $x$ |  $\frac{1}{256^{32}} \le x \le \frac{1}{256^{256}}$, $x \in R$ percent chance of misidentifying the signature, however the odds could go up with a larger file with more possible permutations of bytes. The basic concatenating two files with a signature, however, is breakable by a hex editor if one has ability to identity the binary file type based off of constants in heading. Bunk offsets this by encrypting the data using a [Vigenère cipher](https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher), a one-to-one size encryption method. While it is breakable with considerable effort and computational expense, it is much more secure than the raw binary stored as before. The signature and cipher key are both randomly generated, and seeded from the `b` argument of the hide function. 
