TARGET=main.pdf
COMPILER=pdflatex

FILENAME=$(basename $(notdir $(TARGET)))

all: $(TARGET) $(FILENAME).tex

$(TARGET): $(FILENAME).tex
	$(COMPILER) main.tex

view: $(TARGET)
	evince $<

show: $(TARGET)
	pdfpc $<


rename:
	cp $(TARGET) slide_procedural_stylisation.pdf


clean:
	rm -f $(FILENAME).aux $(FILENAME).log $(FILENAME).bbl $(FILENAME).blg $(FILENAME).bcf $(FILENAME).run.xml $(FILENAME).toc
