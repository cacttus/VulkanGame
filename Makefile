.PHONY: clean All

All:
	@echo "----------Building project:[ est - Debug ]----------"
	@cd "est" && "$(MAKE)" -f  "est.mk"
clean:
	@echo "----------Cleaning project:[ est - Debug ]----------"
	@cd "est" && "$(MAKE)" -f  "est.mk" clean
