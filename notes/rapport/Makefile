TARGET=main.pdf

FILENAME=$(basename $(notdir $(TARGET)))



all: $(TARGET)
	echo $(basename $(notdir $(TARGET)))

$(TARGET): $(FILENAME).tex
	xelatex main.tex

view: $(TARGET)
	evince $<

clean:
	rm -f $(TARGET) $(FILENAME).aux $(FILENAME).log $(FILENAME).toc chapters/*.log chapters/*.aux
