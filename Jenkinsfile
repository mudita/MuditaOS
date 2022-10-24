@Library('PureCI') _

def run_codeql(platform,product,config,commit) {
    withCredentials([string(credentialsId: 'f412733a-851c-4f87-ad24-7da2139a98ca', variable: 'TOKEN')]) {
        sh """#!/bin/bash -e
        pushd "${REPO_WORKSPACE}"
        pushd "build-${product}-${platform}-${config}"

        export CCACHE_DISABLE=1

        echo "Cleaning build to perform full rebuild"
        ninja clean

        echo "Creating CodeQL database"
        /usr/local/codeql/codeql database create \
                                 ../MuditaOS-CodeQL-DB \
                                 --language=cpp \
                                 --command="ninja" \
                                 --overwrite

        popd

        echo "Analyzing CodeQL database"
        /usr/local/codeql/codeql database analyze \
                                 MuditaOS-CodeQL-DB \
                                 codeql/cpp-queries \
                                 --download \
                                 --format=sarif-latest \
                                 --output=MuditaOS-results.sarif \
                                 --threads=30

        cat MuditaOS-results.sarif

        echo "Uploading SARIF results to GitHub"
        echo "${TOKEN}" | /usr/local/codeql/codeql github upload-results \
                                 --repository=mudita/MuditaOS \
                                 --ref=refs/heads/master \
                                 --commit=${commit} \
                                 --sarif=MuditaOS-results.sarif \
                                 --github-auth-stdin"""
    }
}

def build(platform,product,config){
    commit = ""
    if(env.fork == "true"){
        println("setting status for fork")
        commit = env.pr_from_sha
    }
    else{
        commit = env.GIT_COMMIT
    }
    common.setBuildStatus(commit,"Building $product [$config] for $platform", "PENDING");
    env.currentStep = "Building $product [$config] for $platform"
    println("Building $product [$config] for $platform")

    env.CCACHE_DIR="/ccache/"
    sh """#!/bin/bash -e
    pushd "${REPO_WORKSPACE}"
    echo "Product: ${product}, platform: ${platform}"

    echo "./configure.sh ${product} ${platform} ${config} -G Ninja"
    ./configure.sh ${product} ${platform} ${config} -G Ninja"""

    if(platform == "linux"){
        println("Clang Tidy check")
        sh """#!/bin/bash -e
            pushd "${REPO_WORKSPACE}"

            # requires compilation database - must be run after configuration
            ./config/clang_check.sh PurePhone"""
    }

    sh """#!/bin/bash -e
        pushd "${REPO_WORKSPACE}"
        pushd build-${product}-${platform}-${config}
        ninja -j ${JOBS} ${product}
        popd"""

    if(platform == "linux"){
        env.currentStep = "Building $product [$config] for $platform - unit tests"
        println("Building $product [$config] for $platform - unit tests")
        withCredentials([string(credentialsId: 'f412733a-851c-4f87-ad24-7da2139a98ca', variable: 'TOKEN')]) {
            sh """#!/bin/bash -e
                pushd "${REPO_WORKSPACE}"
                pushd build-${product}-${platform}-${config}
                ninja -j ${JOBS} unittests
                popd

                echo "Check for Statics"
                ./tools/find_global_data.py build-${product}-${platform}-${config}/${product}.elf

                pushd build-${product}-${platform}-${config}
                ninja check
                popd"""
        }
    }

    if (platform == "rt1051" && product == "PurePhone") {
        run_codeql(platform,product,config,commit);
    }

    sh """#!/bin/bash -e
    pushd "${REPO_WORKSPACE}"
    rm -r build-${product}-${platform}-${config}

    echo "CCache stats"
    ccache --show-stats"""
}

pipeline {
    agent {
        node {
        label 'jenkins-slave-ccache-ram'
        customWorkspace "/home/jenkins/workspace/MuditaOS"
        }
    }
    options{
        ansiColor('xterm')
        parallelsAlwaysFailFast()
    }
    environment {
        JOBS=30
        PATH="/usr/local/cmake-3.21.3-linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH"
        REPO_WORKSPACE="/home/jenkins/workspace/MuditaOS"

    }

    stages {
        stage('Check for previous running builds') {
            steps {
                script {
                    common.setBuildStatus(env.GIT_COMMIT,"This commit is being built", "PENDING")
                    common.cancelPreviousBuilds()
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
                script{
                    buildSteps.performChecks("false");
                }
            }
        }
        stage('Build RT1051 - PurePhone') {
            steps {
                script{
                    common.checkIfBranchIsRebased("false")
                    build("rt1051","PurePhone","Release")
                }

            }
        }

        stage('Build RT1051 - BellHybrid') {
            steps {
                script{
                    common.checkIfBranchIsRebased("false")
                    build("rt1051","BellHybrid","Release")
                }
            }
        }

        stage('Build Linux - PurePhone') {
            when {
                expression {
                 return !(env.CHANGE_TARGET ==~ /release\/[0-9]+\.[0-9]+\.[0-9]+-bell/)
                 }
            }
            steps {
                script{
                    common.checkIfBranchIsRebased("false")
                    build("linux","PurePhone","Debug")
                }
            }
        }

        stage('Build Linux - BellHybrid') {
            when {
                expression {
                 return !(env.CHANGE_TARGET ==~ /release\/[0-9]+\.[0-9]+\.[0-9]+-pure/)
                 }
            }
            steps {
                script{
                    common.checkIfBranchIsRebased("false")
                    build("linux","BellHybrid","Debug")
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
                    echo "HAL 9000 sleeps here."
                '''
            }
        }
    }
    post {
        success {
            script{
                common.setBuildStatus(env.GIT_COMMIT,"Build succeeded", "SUCCESS");
            }
        }
        failure {
            script{
                common.setBuildStatus(env.GIT_COMMIT,"Build failed: $env.currentStep", "FAILURE");
            }
        }
        cleanup {
            script{
                common.cleanup("false")
            }
        }
    }
}
