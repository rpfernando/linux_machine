# Compiles a .c
function gccc {
    gcc "$1".c -o "$1"
    echo "Compiled $1"
}

# Compiles and run a .c
function gccr {
    gccc "$1"
    echo
    ./"$1"
    rm "$1"
    echo
    echo "Removed $1"
}
