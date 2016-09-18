cat '\\suzhost-18\d$\Old\threshold\es-es\MT\alarm\data\slot.train.tsv' | grep 'time>' | awk 'BEGIN{FS=OFS="\t"}{print $5}' > alarm.time
cat '\\suzhost-18\d$\Old\threshold\es-es\MT\alarm\data\slot.train.tsv' | grep 'date>' | awk 'BEGIN{FS=OFS="\t"}{print $5}' > alarm.date

cat '\\suzhost-18\d$\Old\threshold\es-es\MT\calendar\data\slot.train.tsv' | grep 'time>' | awk 'BEGIN{FS=OFS="\t"}{print $5}' > calendar.time
cat '\\suzhost-18\d$\Old\threshold\es-es\MT\calendar\data\slot.train.tsv' | grep 'date>' | awk 'BEGIN{FS=OFS="\t"}{print $5}' > calendar.date

cat '\\suzhost-18\d$\Old\threshold\es-es\MT\reminder\20150113\data\slot.train.tsv' | grep 'time>' | awk 'BEGIN{FS=OFS="\t"}{print $5}' > reminder.time
cat '\\suzhost-18\d$\Old\threshold\es-es\MT\reminder\20150113\data\slot.train.tsv' | grep 'date>' | awk 'BEGIN{FS=OFS="\t"}{print $5}' > reminder.date

cat alarm.* calendar.* reminder.* | python clean_data.py > train.tsv
