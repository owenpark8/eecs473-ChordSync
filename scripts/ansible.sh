#!/usr/bin/env bash

# Helper script to run Ansible playbooks
# Taken from https://github.com/umrover/mrover-ros (thanks Quintin)

if [ "$#" -le 0 ]; then
    echo "Usage: $0 <playbook> <extra arguments>"
    exit 1
fi

playbook=$1

shift

extra_args="$@"

sudo -v # Ensure Ansible has sudo permission

ansible-playbook -i "localhost," -c local ./ansible/${playbook} ${extra_args}

