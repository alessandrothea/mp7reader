[[ $_ != $0 ]] && echo "Script is being sourced" || echo "Script is a subshell"

function pathadd() {
  # TODO add check for empty path
  # and what happens if $1 == $2
  # Copy into temp variables
  PATH_NAME=$1
  PATH_VAL=${!1}
  if [[ ":$PATH_VAL:" != *":$2:"* ]]; then
    PATH_VAL="$2${PATH_VAL:+":$PATH_VAL"}"
    echo "- $1 += $2"

    # use eval to reset the target
    eval "$PATH_NAME=$PATH_VAL"
  fi

}

CACTUS_ROOT=${CACTUS_ROOT:-"/opt/cactus"}

MP7READER_ROOT=$( readlink -f $(dirname $BASH_SOURCE)/ )

pathadd PATH "${MP7READER_ROOT}/bin"
pathadd LD_LIBRARY_PATH "${MP7READER_ROOT}/lib"
