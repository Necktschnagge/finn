#!/bin/bash

#+++PROJECT-NAME+++
project_name="Finn"
sources_url="https://github.com/Necktschnagge/finn"

sed -i -E "s/___template___project___name___/${project_name}/g" script/*.sh
sed -i -E "s/___template___project___url___/${project_name}/g" CMakeLists.txt
git update-index --chmod +x script/*.sh
#CMakeLists.txt azure-pipelines.yml script/*