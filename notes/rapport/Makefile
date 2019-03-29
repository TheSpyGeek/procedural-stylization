TARGET=main.pdf
COMPILER=pdflatex

FILENAME=$(basename $(notdir $(TARGET)))



all: $(TARGET) $(FILENAME).tex

$(TARGET): $(FILENAME).tex chapters/*.tex
	$(COMPILER) main.tex
	bibtex $(FILENAME)
	$(COMPILER) main.tex
	$(COMPILER) main.tex

view: $(TARGET)
	evince $<

clean:
	rm -f $(FILENAME).aux chapters/*.aux $(FILENAME).log $(FILENAME).bbl $(FILENAME).blg $(FILENAME).bcf $(FILENAME).run.xml $(FILENAME).toc
