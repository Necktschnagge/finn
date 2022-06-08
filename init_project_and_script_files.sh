#!/bin/bash

#+++PROJECT-NAME+++
project_name="Finn"

sed -i -E "s/___template___project___name___/${project_name}/g" script/*.sh
git update-index --chmod +x script/*.sh
#CMakeLists.txt azure-pipelines.yml script/*