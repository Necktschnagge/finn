#!/bin/bash

#+++PROJECT-NAME+++
project_name="Finn"
sources_url="https://github.com/Necktschnagge/finn"

sed -i -E "s/___template___project___name___/${project_name}/g" script/*.sh script/*.ps1 script/*.bat CMakeLists.txt azure-pipelines.yml

sed -i -E "s/___template___project___url___/${project_name}/g" CMakeLists.txt

git update-index --chmod +x script/*.sh
git reset --mixed
git add *
