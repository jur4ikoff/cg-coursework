BUILD_DIR = ./src/build
CMAKE = cmake
MAKE = make
MAIN = report
LATEX = pdflatex

REPORT_SOURCE_PATH = ./report
REPORT_FINAL_PATH = ready/report.pdf


report/report.pdf: $(REPORT_SOURCE_PATH)/$(MAIN).tex
	cd $(REPORT_SOURCE_PATH) && $(LATEX) $(MAIN) > /dev/null
	cd $(REPORT_SOURCE_PATH) && $(LATEX) $(MAIN) > /dev/null
	$(MAKE) -f iu7-algan-cicd-student.mk compress


ready/report.pdf: report/report.pdf
	mkdir -p ./ready
	cp report/report.pdf $(REPORT_FINAL_PATH)

ready/stud-unit-test-report.json:
	@mkdir ready 2>/dev/null || true
	@mkdir $(BUILD_DIR) 2>/dev/null || true
	@cd $(BUILD_DIR) && $(CMAKE) -DENABLE_COVERAGE=ON ..
	@cd $(BUILD_DIR) && $(MAKE)
	python3 src/test_runner.py

# Очистка
.PHONY: clean
clean:
	rm -rf ready
	rm -rf src/build
	rm -rf src/charts
	rm -rf src/data
	rm -rf report/*.log
	rm -rf report/*.aux
	rm -rf report/*.bbl
	rm -rf report/*.blg
	rm -rf report/*.out
	rm -rf report/*.gz
	rm -rf report/*.toc
	echo OK

build:
	@mkdir -p $(BUILD_DIR) 2>/dev/null || true
	@cd $(BUILD_DIR) && $(CMAKE) ..
	@cd $(BUILD_DIR) && $(MAKE)

build_asan:
	@mkdir -p $(BUILD_DIR) 2>/dev/null || true
	@cd $(BUILD_DIR) && $(CMAKE) -DENABLE_ASAN=ON ..
	@cd $(BUILD_DIR) && $(MAKE)

build_ubsan:
	@mkdir -p $(BUILD_DIR) 2>/dev/null || true
	@cd $(BUILD_DIR) && $(CMAKE) -DENABLE_UBSAN=ON ..
	@cd $(BUILD_DIR) && $(MAKE)

# Пересборка (очистка + сборка)
rebuild: clean build

# Запуск проекта (предполагаем, что исполняемый файл имеет то же имя, что и папка проекта)
run: build
	@mkdir -p src/data 2>/dev/null || true
	@cd $(BUILD_DIR) && ./app.exe

# Настройки форматтера
CLANG_FORMAT = clang-format
STYLE = LLVM
SOURCE_EXTENSIONS = *.cpp *.h *.hpp *.cc *.cxx *.hxx
SOURCE_DIRS = src include lib test

# Форматирование всех исходных файлов
format:
	@echo "Formatting source files with $(CLANG_FORMAT) ($(STYLE) style)..."
	@for dir in $(SOURCE_DIRS); do \
		if [ -d "$$dir" ]; then \
			echo "Formatting files in $$dir/"; \
			find "$$dir" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.cc" -o -name "*.cxx" -o -name "*.hxx" \) -exec $(CLANG_FORMAT) -i -style=$(STYLE) {} \; ; \
		fi \
	done
	@echo "✅ Formatting completed."

tests:
	@echo "Starting tests"
	@mkdir -p $(BUILD_DIR) 2>/dev/null || true
	@cd $(BUILD_DIR) && $(CMAKE) ..
	@cd $(BUILD_DIR) && $(MAKE)
	@cd $(BUILD_DIR) && ./tests.exe

compress:
	@echo Compressing pdf
	python3 compress.py

ready/app-cli-debug:
	@echo "Supermegapon"

ready/app-cli-release:
	@echo "Supermegarelease"
