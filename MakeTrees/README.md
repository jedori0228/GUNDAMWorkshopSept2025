# Making Trees from CAFAna: A tutorial

This tutorial will cover how to make ROOT TTrees out of CAFAna, going through various aspects of the `Tree` class and including purely truth based trees as well as reconstructed info and systematics.

The tutorial slides are posted to the SBN doc-db (doc-43378) and will make use of the macro and Jupyter notebook provided here. The .C and .h file are to be used with `sbnana` to make the TTrees, and the Jupyter notebook will be used to investigate the resulting trees.

## Making the tree

`DemoTrees.C` and its header file `DemoTrees.h` are an example of a CAFAna macro, basically a standalone ROOT script that will be run using the `cafe` executable from the `sbnana` package, which is SBN's version of CAFAna, in a very similar format to how I might run a macro using the `root` executable if I were only using ROOT and not the extra libraries provided by CAFAna.

CAFAna needs to be told the files to load in and the details of what sorts of variables and cuts are going to be applied to make Spectrum related objects (histogram outputs) or Tree related objects (mostly TTree formatted outputs), and then CAFAna will handle the event looping for you.

To setup, we will be using v10_01_02 of `sbnana`. While previous versions have the `Tree` class, this is the first version with TruthVar, TruthCut, and Truth Trees available:
```
sh /exp/$(id -ng)/data/users/vito/podman/start_SL7dev_jsl.sh               # launch the container
source /cvmfs/icarus.opensciencegrid.org/products/icarus/setup_icarus.sh   # setup UPS
setup sbnana v10_01_02 -q e26:prof                                         # setup CAFAna
export BEARER_TOKEN_FILE=/tmp/bt_u$(id -u)
htgettoken -a htvaultprod.fnal.gov -i icarus                               # setup token
```
That's all that should be needed to run the macro:
```
cafe -bq DemoTrees.C
```

## Analyzing the output information:

To analyze the outputs of this macro and inspect what we have, we will use Jupyter notebooks with uproot to load in these data. Following the main setup guide for this area and using the Fermilab EAF, you should be good to go. The example notebook here actually just uses a local copy of `icarusplot` (https://github.com/brucehoward-physics/icarusplot) without requiring it to be set in the `$PYTHONPATH` but that way is also fine!
