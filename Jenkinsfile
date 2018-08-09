pipeline {
  agent { label 'streamsx_public' }
  stages {
    stage('Build') {
      steps {
        sh 'ant test'
      }
    }
  }
}
