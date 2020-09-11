#!/bin/bash

#prints VTL version
function vtl() {
  if [ "$1" = "version" ]; then
    cat ~/vtlWorkspace/vtl_server/.vtl_version
  elif [ "$1" = "launcher" ]; then
    echo "Here start launcher job ..."
    echo "... Here end launcher job."
  else
    echo "vtl: incomplete command."
    echo "Try 'vtl --help' for more information."
  fi
}
