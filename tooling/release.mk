# The release targets, which common.mk includes. An app without common.mk, such as an Android
# one, includes this alone, after its own targets:
#
#   RELEASE_SCOPE := release           the release commit's scope, if its commits need one
#   include core/tooling/release.mk

.PHONY: release release-dry major minor patch

# The level comes as a second goal, as in `make release patch`; VERSION=1.2.3 names one.
LEVEL := $(or $(VERSION),$(firstword $(filter major minor patch,$(MAKECMDGOALS))))

release: ## Release from main: make release patch|minor|major, or VERSION=1.2.3
	@RELEASE_SCOPE=$(RELEASE_SCOPE) core/tooling/release.sh $(LEVEL)

release-dry: ## Show what make release would do, changing nothing
	@RELEASE_SCOPE=$(RELEASE_SCOPE) core/tooling/release.sh $(LEVEL) --dry-run

major minor patch:
	@:
