var console = loadmodule('console')
console.println('yo', 'baby', 4.2, null)

var c2 = loadmodule('console')
c2.println('conker 2')

function pando() {
    this.friend = 1
    this.masto = loadmodule('console')
    this.whack = function() {
        this.masto.println('truckman')
    }
}

var c = new pando()
c.whack()

