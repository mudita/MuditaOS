@Library('PureCI') _

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
                    buildSteps.build("rt1051","PurePhone","Release")
                }
            }
        }

        stage('Build RT1051 - BellHybrid') {
            steps {
                script{
                    common.checkIfBranchIsRebased("false")
                    buildSteps.build("rt1051","BellHybrid","Release")
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
                    buildSteps.build("linux","PurePhone","Debug")
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
                    buildSteps.build("linux","BellHybrid","Debug")
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
