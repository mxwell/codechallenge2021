import datetime
import logging
import sys

# Checked with https://omniglot.com/language/time/days.htm
# Exceptions:
# - sunday in SK: nedel'a -> nedeľa
# - saturday in RO: sîmbătă -> sâmbătă
WEEKDAYS_BY_LANGUAGE = {
    "FR": ["lundi", "mardi", "mercredi", "jeudi", "vendredi", "samedi", "dimanche"],
    "EN": ["monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"],
    "VI": ["thứ hai", "thứ ba", "thứ tư", "thứ năm", "thứ sáu", "thứ bảy", "chủ nhật"],
    "DK": ["mandag", "tirsdag", "onsdag", "torsdag", "fredag", "lørdag", "søndag"],
    "CZ": ["pondělí", "úterý", "středa", "čtvrtek", "pátek", "sobota", "neděle"],
    "SK": ["pondelok", "utorok", "streda", "štvrtok", "piatok", "sobota", "nedeľa"],
    "FI": ["maanantai", "tiistai", "keskiviikko", "torstai", "perjantai", "lauantai", "sunnuntai"],
    "CA": ["dilluns", "dimarts", "dimecres", "dijous", "divendres", "dissabte", "diumenge"],
    "RO": ["luni", "marţi", "miercuri", "joi", "vineri", "sâmbătă", "duminică"],
    "IT": ["lunedì", "martedì", "mercoledì", "giovedì", "venerdì", "sabato", "domenica"],
    "NL": ["maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag", "zondag"],
    "SI": ["ponedeljek", "torek", "sreda", "četrtek", "petek", "sobota", "nedelja"],
    "SE": ["måndag", "tisdag", "onsdag", "torsdag", "fredag", "lördag", "söndag"],
    "PL": ["poniedziałek", "wtorek", "środa", "czwartek", "piątek", "sobota", "niedziela"],
    "HU": ["hétfő", "kedd", "szerda", "csütörtök", "péntek", "szombat", "vasárnap"],
    "DE": ["montag", "dienstag", "mittwoch", "donnerstag", "freitag", "samstag", "sonntag"],
    "GR": ["δευτέρα", "τρίτη", "τετάρτη", "πέμπτη", "παρασκευή", "σάββατο", "κυριακή"],
    "ES": ["lunes", "martes", "miércoles", "jueves", "viernes", "sábado", "domingo"],
    "IS": ["mánudagur", "þriðjudagur", "miðvikudagur", "fimmtudagur", "föstudagur", "laugardagur", "sunnudagur"],
    "RU": ["понедельник", "вторник", "среда", "четверг", "пятница", "суббота", "воскресенье"],
}


def parse_date(date_str):
    parts = date_str.split("-")
    assert len(parts) == 3
    if len(parts[0]) == 4 and len(parts[1]) == 2 and len(parts[2]) == 2:
        return [int(part) for part in parts]
    if len(parts[0]) == 2 and len(parts[1]) == 2 and len(parts[2]) == 4:
        return reversed([int(part) for part in parts])
    raise Exception("bad date: {}".format(date_str))


def handle_case():
    raw_input = input().strip()
    date_str, language_str = raw_input.split(":")
    yy, mm, dd = parse_date(date_str)
    try:
        dt = datetime.datetime(yy, mm, dd)
    except ValueError:
        print("INVALID_DATE")
        return
    if language_str not in WEEKDAYS_BY_LANGUAGE:
        print("INVALID_LANGUAGE")
        return
    weekday = dt.weekday()
    print(WEEKDAYS_BY_LANGUAGE[language_str][weekday])


def main():
    for k, v in WEEKDAYS_BY_LANGUAGE.items():
        assert len(v) == 7

    n = int(input().strip())
    for case in range(1, n + 1):
        print("Case #{}: ".format(case), end="")
        handle_case()
    return 0


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    sys.exit(main())