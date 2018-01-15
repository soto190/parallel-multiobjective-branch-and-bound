# README #

This README would normally document whatever steps are necessary to get your application up and running.

### What is this repository for? ###

* Quick summary
* Version
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

### How do I get set up? ###


### Dependencies ###
Message Passing Interface (MPI)
Intel Threading Building Blocks (TBB)

* Summary of set up
* Configuration
Remember to set the intel bins, libs and includes in 'profil'e and 'bashrc'.

* In 'bashrc' the libraries and includes:

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/intel/debugger_2017/libipt/intel64/lib/:/opt/intel/compilers_and_libraries_2017/linux/tbb/lib/intel64_lin/gcc4.7:/opt/intel/compilers_and_libraries_2017/linux/tbb/include

export PYTHONHOME=$PYTHONHOME:/opt/intel/debugger_2017/python/intel64
export PYTHONPATH=$PYTHONPATH:/opt/intel/debugger_2017/python/intel64/lib/python2.7

* In 'profile' the binaries:
PATH="$HOME/bin:$HOME/.local/bin:$PATH:/opt/intel/bin"



* Database configuration
* How to run tests
* Deployment instructions

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact