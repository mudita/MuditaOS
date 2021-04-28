pipeline {
  agent {
    node {
      label 'jenkins-slave-ccache'
    }

  }
  environment {
    JOBS=5
  }
  stages {

    stage('Initial checks') {
        environment {
            GITHUB_BASE_REF="${pullRequest.base}"
            GITHUB_HEAD_REF="${pullRequest.headRef}"
        }
        steps {
            echo "Commit Message check"
            sh '''#!/bin/bash -e
pushd ${WORKSPACE}
./tools/check_commit_messages.py
popd'''
            echo "Copyright notice check"
            sh '''#!/bin/bash -e
pushd ${WORKSPACE}
./config/license_header_check.sh --ci --check-only
popd'''
            echo "Style checking"
            sh '''#!/bin/bash -e
pushd ${WORKSPACE}
./config/style_check_hook.sh --last
popd'''
        }
    }
    stage('Build') {
        parallel {
        stage('Build RT1051') {
            agent {
                node {
                    label 'jenkins-slave-ccache'
                }         
            }
            steps {
                sh '''#!/bin/bash -e
PATH="/usr/local/cmake-3.19.5-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
export JOBS=${JOBS:-6}
export CCACHE_DIR=/ccache/RT1051

echo "JOBS=${JOBS}"
echo "\'workspace dir:${WORKSPACE}\'"

pushd "${WORKSPACE}"
echo "./configure.sh rt1051 Release -G Ninja"

./configure.sh rt1051 Release -G Ninja

pushd build-rt1051-Release
ninja -j ${JOBS}
popd
popd'''
                echo "CCache stats"
                sh '''#!/bin/bash
export CCACHE_DIR=/ccache/RT1051
ccache --show-stats'''
            }
        }

        stage('Build Linux') {
            agent {
                node {
                    label 'jenkins-slave-ccache'
                }         
            }

            steps {
                echo "Build"
                sh '''#!/bin/bash -e
PATH="/usr/local/cmake-3.19.5-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
export JOBS=${JOBS:-6}
export CCACHE_DIR=/ccache/Linux

echo "JOBS=${JOBS}"
echo "\'workspace dir:${WORKSPACE}\'"

pushd "${WORKSPACE}"
echo "./configure.sh linux Debug -G Ninja"

./configure.sh linux Debug -G Ninja

pushd build-linux-Debug
ninja -j ${JOBS}
ninja -j ${JOBS} unittests
popd
popd'''
                echo "CCache stats"
                sh '''#!/bin/bash
export CCACHE_DIR=/ccache/Linux
ccache --show-stats'''
                echo "Check for Statics"
                sh '''#!/bin/bash -e
pushd "${WORKSPACE}"
./tools/find_global_data.py build-linux-Debug/PurePhone.elf
popd'''
                echo "Run Unit Tests"
                sh '''#!/bin/bash -e
PATH="/usr/local/cmake-3.19.5-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
export JOBS=${JOBS:-6}
export CCACHE_DIR=/ccache/Linux
echo "JOBS=${JOBS}"
echo "\'workspace dir:${WORKSPACE}\'"

pushd "${WORKSPACE}"
./tools/run_unittests.sh enabled_unittests
./tools/check_unittests.sh
pushd build-linux-Debug
./googletest-gui
popd
popd'''
                echo "CCache stats"
                sh '''#!/bin/bash
export CCACHE_DIR=/ccache/Linux
ccache --show-stats'''
            }
        }
        }
    }

  }
}
