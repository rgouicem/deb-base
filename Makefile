SCRIPTS_DIR = ./scripts/
DOCS_DIR = ./docs/
RESOURCES_DIR = ./resources/

all: build_package build_docs

build_package: $(SCRIPTS_DIR)build_deb.sh $(SCRIPTS_DIR)config_parser.pl $(SCRIPTS_DIR)build_config.yml $(SCRIPTS_DIR)correct_lintian.pl
	cd $(SCRIPTS_DIR) && ./build_deb.sh

build_docs: $(SCRIPTS_DIR)build_deb.sh $(SCRIPTS_DIR)build_config.yml $(SCRIPTS_DIR)correct_lintian.pl $(RESOURCES_DIR)languages.json $(RESOURCES_DIR)docco.css
	docco -L $(RESOURCES_DIR)languages.json -c $(RESOURCES_DIR)docco.css -o $(DOCS_DIR) -l linear $(SCRIPTS_DIR)build_deb.sh
	docco -L $(RESOURCES_DIR)languages.json -c $(RESOURCES_DIR)docco.css -o $(DOCS_DIR) -l linear $(SCRIPTS_DIR)correct_lintian.pl
	docco -L $(RESOURCES_DIR)languages.json -c $(RESOURCES_DIR)docco.css -o $(DOCS_DIR) -l linear $(SCRIPTS_DIR)build_config.yml

clean_docs:
	rm -rf $(DOCS_DIR)

.PHONY: all build_package build_docs clean_docs