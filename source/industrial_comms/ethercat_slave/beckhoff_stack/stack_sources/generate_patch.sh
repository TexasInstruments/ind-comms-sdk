git checkout demo_mode_stack_patch
git rebase master
git checkout 8_sm_branch
git rebase master
git checkout mcu_plus_sdk
git rebase master

echo "generating TI_ECAT.patch"
git checkout master
git diff original_stack_reference..HEAD ':!*.sh' ':!ReadMe.txt' > TI_ECAT.patch
sed -i TI_ECAT.patch -e 's&diff\s--git\sa/.*\sb/&Index: &g' -e 's&index\s.*\s.*&===================================================================&g' -e 's&---\sa/&--- &g' -e 's&+++\sb/&+++ &g'


echo "generating TI_ECAT_DEMO.patch"
git checkout demo_mode_stack_patch
git diff original_stack_reference..HEAD ':!*.sh' ':!ReadMe.txt' > TI_ECAT_DEMO.patch
sed -i TI_ECAT_DEMO.patch -e 's&diff\s--git\sa/.*\sb/&Index: &g' -e 's&index\s.*\s.*&===================================================================&g' -e 's&---\sa/&--- &g' -e 's&+++\sb/&+++ &g'


echo "generating 8SM_TI_ECAT.patch"
git checkout 8_sm_branch
git diff original_stack_reference..HEAD ':!*.sh' ':!ReadMe.txt' > 8SM_TI_ECAT.patch
sed -i 8SM_TI_ECAT.patch -e 's&diff\s--git\sa/.*\sb/&Index: &g' -e 's&index\s.*\s.*&===================================================================&g' -e 's&---\sa/&--- &g' -e 's&+++\sb/&+++ &g'

echo "generating TI_ECAT_MCUSDK.patch"
git checkout mcu_plus_sdk
git diff original_stack_reference..HEAD ':!*.sh' ':!ReadMe.txt' > TI_ECAT_MCUSDK.patch
sed -i TI_ECAT_MCUSDK.patch -e 's&diff\s--git\sa/.*\sb/&Index: &g' -e 's&index\s.*\s.*&===================================================================&g' -e 's&---\sa/&--- &g' -e 's&+++\sb/&+++ &g'
