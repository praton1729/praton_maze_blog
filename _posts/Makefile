SOURCES=$(wildcard *.md)

WIKI=$(SOURCES:.md=.wiki)

WIKI_TARGETS= $(addprefix ../wiki/, $(WIKI))

default: all

all: $(WIKI_TARGETS)

wiki/%.wiki: %.md
	pandoc -w mediawiki $< -o $@

clean:
	rm wiki/*
