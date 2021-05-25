def cancelPreviousBuilds() {

    def jobName = env.JOB_NAME
    def buildNumber = env.BUILD_NUMBER.toInteger()
    def currentJob = Jenkins.instance.getItemByFullName(jobName)

    for (def build : currentJob.builds) {
        def exec = build.getExecutor()

        /* Kill older jobs */
        if (build.isBuilding() && build.number.toInteger() < buildNumber && exec != null) {
            def cause = { "Job stoped by #${buildNumber}" as String} as CauseOfInterruption
            exec.interrupt(Result.ABORTED, cause)
            println("Aborting previous build:#${build.number}")
        }
    }
}
pipeline {
  agent {
    node {
      label 'jenkins-slave-ccache-ram'
    }
  }
  options{
    ansiColor('xterm')
    parallelsAlwaysFailFast()
  }
  environment {
    JOBS=15
  }
  stages {
    stage('Check for previous running builds') {
        
        steps {
            script {
                cancelPreviousBuilds()
            }
        }
    }

    stage('Initial checks') {
        when {
            changeRequest()
        }
        environment {
            GITHUB_BASE_REF="${pullRequest.base}"
            GITHUB_HEAD_REF="${pullRequest.headRef}"
        }
        steps {
            echo "Check if branch needs rebasing"
            sh '''#!/bin/bash -e
pushd ${WORKSPACE}

if [[ $(git log origin/${CHANGE_TARGET}..HEAD) ]]; then
    echo "Branch OK"
else
    echo "Branch is not rebased. Exiting"
    exit 1
fi


popd'''
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
        when {
            changeRequest()
        }
        parallel {
        stage('Build RT1051') {
            agent {
                node {
                    label 'jenkins-slave-ccache-ram'
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
                    label 'jenkins-slave-ccache-ram'
                }
            }

            environment {
                PATH="/usr/local/cmake-3.19.5-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
                CCACHE_DIR="/ccache/Linux"
            }

            steps {
                echo "Build"
                sh '''#!/bin/bash -e
echo "JOBS=${JOBS}"
pushd "${WORKSPACE}"
echo "./configure.sh linux Debug -G Ninja"

./configure.sh linux Debug -G Ninja

pushd build-linux-Debug
ninja -j ${JOBS}
popd'''
                echo "Build Unit Tests"
                sh '''#!/bin/bash -e
pushd "${WORKSPACE}"
pushd build-linux-Debug
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
export JOBS=${JOBS:-6}
echo "JOBS=${JOBS}"
pushd "${WORKSPACE}"
./tools/run_unittests.sh enabled_unittests
./tools/check_unittests.sh
pushd build-linux-Debug
./googletest-gui
popd
popd'''
            }
        }
        }
    }
    stage('master-jobs') {
        when {
            branch 'master'
        }
        steps {
            echo "run some tests"
            sh '''#!/bin/bash
                echo "HALL 9000 sleeps here."
            '''
            
        }
    }
  }
  post {
    always {
        cleanWs()
    }
  }
}
