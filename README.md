
# bunk

# Overview
Bunk is a CLI to hide binaries in one another. It does so by 'bunking' the two binaries together, and seperating the bytes by a distinguishable signature so they are retrieveable even without a distinct start point. 

# Useage
`hide exec.exe photo.jpg out.jpg` <br>
`find out.jpg out.exe`
