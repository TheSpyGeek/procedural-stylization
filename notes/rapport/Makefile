TARGET=main.pdf

FILENAME=$(basename $(notdir $(TARGET)))



all: $(TARGET) $(FILENAME).tex

$(TARGET): $(FILENAME).tex chapters/*.tex
	pdflatex main.tex

view: $(TARGET)
	evince $<

clean:
	rm -f $(TARGET) $(FILENAME).aux $(FILENAME).log $(FILENAME).toc chapters/*.log chapters/*.aux
