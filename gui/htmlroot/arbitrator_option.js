let ArbitratorOptionComponent = {
    name: 'arbitrator-option',
    template: '#arbitrator-option-template',
    components: {
        'icon-priority': iconPriorityComponent,
        'icon-sequence': iconSequenceComponent,
        'icon-cost': iconCostComponent,
        'icon-random': iconRandomComponent,
        'icon-merging': iconMergingComponent,
        'icon-failed': iconFailedComponent,
        'icon-delayed': iconDelayedComponent,
        'icon-reboot': iconRebootComponent,
        'icon-fallback': iconFallbackComponent
    },
    props: {
        name: { type: String, required: true },
        type: { type: String, required: true },
        isActive: { type: Boolean, default: false },
        isInvocable: { type: Boolean, required: true },
        isCommitted: { type: Boolean, required: true },
        verificationResult: { type: String, default: null },
        utility: { type: Number, default: null },
        flags: { type: Object, default: null },
        options: { type: Object, default: null },
        activeOption: { type: Number, default: null },
        activeOptions: { type: Number, default: [] },
    },
    computed: {
        failedVerification: function () {
            return this.verificationResult && this.verificationResult == "failed";
        },
        isDelayed: function () {
            return this.verificationResult && this.verificationResult == "noTimelyResponse";
        },
        isRebooting: function () {
            return this.flags && this.flags.includes("REBOOTING");
        },
        isDeactivated: function () {
            return this.flags && this.flags.includes("DEACTIVATED");
        },
        isFallbackOption: function () {
            return this.flags && this.flags.includes("FALLBACK");
        },
        optionMarginX: function () {
            return 100;
        },
        optionMarginY: function () {
            return 10;
        },
        optionInnerHeight: function () {
            return 40;
        },
        optionInnerWidth: function () {
            return 220;
        },
        optionOuterHeight: function () {
            return this.optionInnerHeight + this.optionMarginY;
        },
        optionOuterWidth: function () {
            return this.optionInnerWidth + this.optionMarginX;
        },
        minOptionsCost: function () {
            if (this.options) {
                minCost = Math.min(...this.options.map(o => o.cost ? o.cost : Infinity), Infinity);
                if (minCost < Infinity) {
                    return minCost;
                }
            }
            return -Infinity;
        },
        maxOptionsCost: function () {
            if (this.options) {
                maxCost = Math.max(...this.options.map(o => o.cost ? o.cost : -Infinity), -Infinity);
                if (maxCost > -Infinity) {
                    return maxCost;
                }
            }
            return Infinity;
        },
        beautifiedName: function () {
            return this.name.split(/(?=[A-Z])/).join(' ');
        }
    },
    methods: {
        isOptionActive: function (option) {
            if (option === this.options[this.activeOption]) {
                return true;
            }

            for (activeOption of this.activeOptions) {
                if (option === this.options[activeOption]) {
                    return true;
                }
            }
            return false;
        },
        isOptionInvocable: function (option) {
            return option.behavior.invocationCondition;
        },
        isOptionCommitted: function (option) {
            return option.behavior.commitmentCondition;
        },
        isOptionDeactivated: function (option) {
            return option.flags && option.flags.includes("DEACTIVATED");
        },
        optionFailedVerification: function (option) {
            return option.verificationResult && option.verificationResult == "failed";
        },
        isOptionDelayed: function (option) {
            return option.verificationResult && option.verificationResult == "noTimelyResponse";
        },
        optionRx: function (options) {
            if (options == null || options === undefined || options.length == 0) {
                // This component is a behavior option → rounded edges
                return 10;
            } else {
                // This component is an arbitrator → square edges
                return 0;
            }
        },
        optionRy: function () {
            return this.optionRx();
        },
        optionY: function (index) {
            return index * this.optionOuterHeight - this.options.length / 2 * this.optionOuterHeight + this.optionOuterHeight / 2;
        },
        normalizedUtility: function (cost, minCost, maxCost) {
            costRange = maxCost - minCost;
            minPadding = 0.2;
            maxPadding = 0.2;
            costScale = (1 - minPadding - maxPadding) / costRange;

            normalizedCost = (cost - minCost) * costScale + minPadding;

            return 1 - normalizedCost;
        }
    }
}