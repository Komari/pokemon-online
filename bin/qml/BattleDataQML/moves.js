var components = {};

var effects = {
};

effects[BattleData.LeechSeed] = "Moves/LeechSeed.qml";
effects[BattleData.Earthquake] = "Moves/Earthquake.qml";
effects[BattleData.StealthRock] = "Moves/HiddenPebbles.qml";
effects[BattleData.ToxicSpikes] = "Moves/ToxicSpikes.qml";
effects[BattleData.Spikes] = "Moves/Spikes.qml";
effects[BattleData.RapidSpin] = "Moves/RapidSpin.qml";
effects[BattleData.QuickAttack] = "Moves/QuickAttack.qml";
effects[BattleData.U_turn] = "Moves/UTurn.qml";

function useAttack(attacker, attack, defender) {
    launchMove(attacker, attack, defender);
}

function launchMove(attacker, attack, defender) {
    var key = attack;
    if (! (key in effects)) {
        console.log("Error: Move effect not found: " + key);
        return;
    }

    var c;
    if (! (key in components) ) {
        var component = Qt.createComponent(effects[key]);

        if (component.status != Component.Ready) {
            console.log("Failed loading components " + key + " (" + effects[key] + ")");
            return;
        }

        c = components[key] = component;
    } else {
        c = components[key];
    }

    var obj = c.createObject(attacker, {"attacker":attacker, "attack":attack, "target":defender});
    battle.scene.pause();
    obj.finished.connect(function() {battle.scene.unpause();obj.destroy();});
    obj.start();
}
