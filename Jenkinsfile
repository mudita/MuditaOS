pipeline {
  agent {
    node {
      label 'jenkins-slave'
    }

  }
  stages {
    stage('Build RT1051') {
      steps {
        sh '''#!/bin/bash -e
PATH="/usr/local/cmake-3.19.5-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
export JOBS=6
echo "JOBS=${JOBS}"
echo "\'workspace dir:${WORKSPACE}\'"

pushd "${WORKSPACE}"
echo "./configure.sh rt1051 Release -G Ninja"

./configure.sh rt1051 Release -G Ninja

pushd build-rt1051-Release
ninja -j ${JOBS}
popd
popd'''
      }
    }

    stage('Build Linux') {
      steps {
        sh '''#!/bin/bash -e
PATH="/usr/local/cmake-3.19.5-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
export JOBS=6
echo "JOBS=${JOBS}"
echo "\'workspace dir:${WORKSPACE}\'"

pushd "${WORKSPACE}"
echo "./configure.sh rt1051 Release -G Ninja"

./configure.sh linux Release -G Ninja

pushd build-linux-Release
ninja -j ${JOBS}
popd
popd'''
      }
    }

  }
}