NPDM_JSON := $(NPDM_JSON_PATH)/$(NPDM_JSON)

.PHONY: npdm-json

npdm-json:
	@$(PYTHON) $(SCRIPTS_PATH)/make-npdm-json.py
