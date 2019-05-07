#!/bin/bash

PROGRAM=run_coalescence.sh
VERSION=1.0

DESCRIPTION=$(cat << __DESCRIPTION__

__DESCRIPTION__
)

OPTIONS=$(cat <<__OPTIONS__

## OPTIONS ##
   	  -ind	(Input) Total individus
	  -name (Output) Name of outputs files
	  -size (Input) Size of sample
	  -folder Name of folder for output files
__OPTIONS__
)

# default options :
individus=5 #5 individu by default
name=newick #"newick" is the default name of output file
size=1 #size of one in sample
folder="output"

# -h :

USAGE ()
{
    cat << __USAGE__
$PROGRAM version $VERSION:
$DESCRIPTION
$OPTIONS

__USAGE__
}

# Bad options
BAD_OPTION ()
{
  echo
  echo "Unknown option "$1" found on command-line"
  echo "It may be a good idea to read the usage:"
  echo "white ./run_coalescence.sh -h to be helped :"
  echo -e $USAGE
  exit 1
}

# Check options
while [ -n "$1" ]; do
    case $1 in
	-h) USAGE; exit 0;;
	-ind) individus=$2; shift 2; continue;;
	-name) name=$2; shift 2; continue;;
	-size) size=$2; shift 2; continue;;
	-folder) folder=$2; shift 2; continue;;
	*) BAD_OPTION $1;;
    esac
done

# create output folder
create_output_folder()
{
    if [[ ! -d "$folder" ]]; then
	mkdir $folder
    fi
}

# Main
STARTIME=$(date +%s)
rm -rf $folder/*nwk
create_output_folder
./coalescent_trees $individus $folder/$name $size
ENDTIME=$(date +%s)
echo "It took $(($ENDTIME - $STARTIME)) seconds to complet this task"
