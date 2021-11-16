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
        customWorkspace "/home/jenkins/workspace/${JOB_NAME}/${BUILD_NUMBER}"
        }
    }
    options{
        ansiColor('xterm')
        parallelsAlwaysFailFast()
    }
    environment {
        JOBS=30
        PATH="/usr/local/cmake-3.21.3-linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
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
                withCredentials([string(credentialsId: 'f412733a-851c-4f87-ad24-7da2139a98ca', variable: 'TOKEN')]) {
                    sh ''' #!/bin/bash -e
                    git config --add --global user.apitoken ${TOKEN}
                    '''
                }
                echo "install additional python dependencies"
                sh '''#!/bin/bash -e
                python3 -m pip install ghapi
                '''
                echo "Check if branch needs rebasing"
                sh '''#!/bin/bash -e
                pushd ${WORKSPACE}
                if [ $(git rev-list --count origin/${CHANGE_TARGET}...`(git merge-base ${GIT_COMMIT}  origin/${CHANGE_TARGET})`) = 0 ]; then
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
        stage('Build RT1051') {
            environment {
                CCACHE_DIR="/ccache/"
                XDG_CACHE_HOME="/clang-cache"
            }
            steps {
                echo "Check if branch needs rebasing"
                sh '''#!/bin/bash -e
                pushd ${WORKSPACE}
                if [ $(git rev-list --count origin/${CHANGE_TARGET}...`(git merge-base ${GIT_COMMIT}  origin/${CHANGE_TARGET})`) = 0 ]; then
                    echo "Branch OK"
                else
                    echo "Branch is not rebased. Exiting"
                    exit 1
                fi
                popd'''

                sh '''#!/bin/bash -e
                export JOBS=${JOBS}
                export CCACHE_DIR=/ccache/
                echo "JOBS=${JOBS}"

                echo "cmake path=$(which cmake)"
                echo "\'workspace dir:${WORKSPACE}\'"

                pushd "${WORKSPACE}"

                echo "./configure.sh pure rt1051 Release -G Ninja"
                ./configure.sh pure rt1051 Release -G Ninja

                pushd build-PurePhone-rt1051-Release
                ninja -j ${JOBS} PurePhone-boot.bin
                popd
                rm -r build-PurePhone-rt1051-Release

                echo "./configure.sh bell rt1051 Release -G Ninja"
                ./configure.sh bell rt1051 Release -G Ninja

                pushd build-BellHybrid-rt1051-Release
                ninja -j ${JOBS} BellHybrid-boot.bin
                popd

                echo "disk usage before removal:"
                du -h --max-depth=1 .

                rm -r build-BellHybrid-rt1051-Release

                echo "disk usage after removal:"
                du -h --max-depth=1 .

                popd'''
                echo "CCache stats"
                sh '''#!/bin/bash
                export CCACHE_DIR=/ccache/
                ccache --show-stats'''
                }
        }

        stage('Build Linux - Pure') {
            environment {
                CCACHE_DIR="/ccache/"
                XDG_CACHE_HOME="/clang-cache"

            }
            when {
                expression {
                 return !(env.CHANGE_TARGET ==~ /release\/[0-9]+\.[0-9]+\.[0-9]+-bell/)
                 }
            }
            steps {
                echo "Check if branch needs rebasing"
                sh '''#!/bin/bash -e
                pushd ${WORKSPACE}
                if [ $(git rev-list --count origin/${CHANGE_TARGET}...`(git merge-base ${GIT_COMMIT}  origin/${CHANGE_TARGET})`) = 0 ]; then
                    echo "Branch OK"
                else
                    echo "Branch is not rebased. Exiting"
                    exit 1
                fi
                popd'''

                echo "Configure"
                sh '''#!/bin/bash -e
                echo "JOBS=${JOBS}"
                pushd "${WORKSPACE}"

                echo "./configure.sh pure linux Debug -G Ninja"
                ./configure.sh pure linux Debug -G Ninja
                popd'''

                echo "Clang Tidy check"
                /* requires compilation database - must be run after configuration */
                sh '''#!/bin/bash -e
                pushd ${WORKSPACE}
                ./config/clang_check.sh PurePhone
                popd'''

                echo "Build"
                sh '''#!/bin/bash -e
                echo "JOBS=${JOBS}"
                pushd "${WORKSPACE}"

                pushd build-PurePhone-linux-Debug
                ninja -j ${JOBS} PurePhone-boot.bin
                popd
                popd'''

                echo "Build Unit Tests"
                sh '''#!/bin/bash -e
                pushd "${WORKSPACE}"
                pushd build-PurePhone-linux-Debug
                ninja -j ${JOBS} unittests
                popd'''

                echo "CCache stats"
                sh '''#!/bin/bash
                export CCACHE_DIR=/ccache/
                ccache --show-stats'''

                echo "Check for Statics"
                sh '''#!/bin/bash -e
                pushd "${WORKSPACE}"
                ./tools/find_global_data.py build-PurePhone-linux-Debug/PurePhone.elf
                popd'''

                echo "Run Unit Tests"
                sh '''#!/bin/bash -e
                export JOBS=${JOBS}
                echo "JOBS=${JOBS}"

                pushd "${WORKSPACE}"
                pushd build-PurePhone-linux-Debug
                ninja check -j ${JOBS}
                popd

                echo "disk usage before removal:"
                du -h --max-depth=1 .

                rm -r build-PurePhone-linux-Debug

                echo "disk usage after removal:"
                du -h --max-depth=1 .
                popd'''
                }
            }


        stage('Build Linux - Bell') {
            environment {
                CCACHE_DIR="/ccache/"
                XDG_CACHE_HOME="/clang-cache"
            }
            when {
                expression {
                 return !(env.CHANGE_TARGET ==~ /release\/[0-9]+\.[0-9]+\.[0-9]+-pure/)
                 }
            }
            steps {
                echo "Check if branch needs rebasing"
                sh '''#!/bin/bash -e
                pushd ${WORKSPACE}
                if [ $(git rev-list --count origin/${CHANGE_TARGET}...`(git merge-base ${GIT_COMMIT}  origin/${CHANGE_TARGET})`) = 0 ]; then
                    echo "Branch OK"
                else
                    echo "Branch is not rebased. Exiting"
                    exit 1
                fi
                popd'''

                echo "Configure"
                sh '''#!/bin/bash -e
                echo "JOBS=${JOBS}"
                pushd "${WORKSPACE}"
                echo "./configure.sh bell linux Debug -G Ninja"

                ./configure.sh bell linux Debug -G Ninja

                popd'''
                echo "Clang Tidy check"
                /* requires compilation database - must be run after configuration */
                sh '''#!/bin/bash -e
                pushd ${WORKSPACE}
                ./config/clang_check.sh BellHybrid
                popd'''

                echo "Build"
                sh '''#!/bin/bash -e
                echo "JOBS=${JOBS}"
                pushd "${WORKSPACE}"

                pushd build-BellHybrid-linux-Debug
                ninja -j ${JOBS} BellHybrid-boot.bin
                popd

                popd'''

                echo "Build Unit Tests"
                sh '''#!/bin/bash -e
                pushd "${WORKSPACE}"

                pushd "${WORKSPACE}"
                pushd build-BellHybrid-linux-Debug
                ninja -j ${JOBS} unittests
                popd
                popd'''

                echo "CCache stats"
                sh '''#!/bin/bash
                export CCACHE_DIR=/ccache/
                ccache --show-stats'''

                echo "Check for Statics"
                sh '''#!/bin/bash -e
                pushd "${WORKSPACE}"
                ./tools/find_global_data.py build-BellHybrid-linux-Debug/BellHybrid.elf
                popd'''

                echo "Run Unit Tests"
                sh '''#!/bin/bash -e
                export JOBS=${JOBS}
                echo "JOBS=${JOBS}"
                pushd "${WORKSPACE}"

                pushd build-BellHybrid-linux-Debug
                ninja check -j ${JOBS}
                popd

                echo "disk usage before removal:"
                du -h --max-depth=1 .

                rm -r build-BellHybrid-linux-Debug

                echo "disk usage after removal:"
                du -h --max-depth=1 .
                popd'''
                }
            }

        stage('master-jobs') {
            when {
                branch 'master'
            }
            steps {
                echo "run some tests"
                sh '''#!/bin/bash
                    echo "HAL 9000 sleeps here."
                '''
            }
        }

    }
    post {
        cleanup {
            deleteDir()
            dir("${workspace}@tmp") {
                deleteDir()
            }
            dir("${workspace}@script") {
                deleteDir()
            }
        }
    }

}
