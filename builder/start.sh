#!/bin/bash

docker run --rm -ti -v "/dev/bus/usb:/dev/bus/usb" -v "$(realpath ..):/app" --privileged  ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder:fd51be0a8aa31e100dbe19bb127048175ab2b005