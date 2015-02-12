#!/usr/bin/env bash

# update apt
sudo apt-get update

sudo apt-get install curl -y

cat /var/vagrant/ci_jobs/bash_functions.sh >> ~/.bash_profile

echo "cd /var/vagrant/src" >> ~/.bash_profile
