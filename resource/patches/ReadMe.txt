How to apply the patch:

(1) Switch to the Geant4 directory which you want to patch, e.g.
cd $MEGALIB/external/geant4_v10.00.p04

(2) Apply the patch
patch -p1 < $MEGALIB/resource/patches/Geant4_v10.00.p04.patch 

(3) Recompile geant4
cd $MEGALIB/external/geant4_v10.00.p04/geant4.10.00.p04-build
make install

(4) Rename and link
cd $MEGALIB/external
mv geant4_v10.00.p04 geant4_v10.00.p04_patched
ln -s geant4_v10.00.p04_patched geant4_v10.00.p04

(5)
Done! ... there is no need to recompile MEGAlib


To create a patch:

(1)
For example
diff -rupN root_v5.34.32_unpatched/root_v5.34.32-source root_v5.34.32_patched/root_v5.34.32-source

(2)
Append it to an existing one if there is any


