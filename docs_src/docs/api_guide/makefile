
export INDUSTRIAL_COMMUNICATIONS_SDK_PATH=$(abspath ../../..)
include $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/imports.mak

DOXYGEN ?= doxygen
export DEVICE ?= am64x
export API_GUIDE_OUT_DIR=$(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/docs/api_guide_$(DEVICE)

all:
	$(MAKE) -C $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/docs_src/docs/api_guide/doxy_samples -f makefile.$(DEVICE).r5f.ti-arm-clang all
	$(MKDIR) $(API_GUIDE_OUT_DIR)
	$(DOXYGEN) doxygen.cfg 2> doxy_warnings_$(DEVICE).txt
	cd theme && $(COPY) *.png $(API_GUIDE_OUT_DIR)

clean:
	$(MAKE) -C $(INDUSTRIAL_COMMUNICATIONS_SDK_PATH)/docs_src/docs/api_guide/doxy_samples -f makefile.$(DEVICE).r5f.ti-arm-clang clean
	$(RMDIR) $(API_GUIDE_OUT_DIR)

help:
	@echo "# Supported build targets:"
	@echo "# ========================"
	@echo "# $(MAKE) -s all DEVICE=<device>"
	@echo "# $(MAKE) -s clean DEVICE=<device>"
	@echo "# $(MAKE) -s help"
	@echo "#"

.PHONY: all clean help
